#include <pixpq/pixpq.hpp>

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>

namespace pixpq {

  location::location(float x, float y, float z) : x(x), y(y), z(z) { }

  db::db() : conninfo(""), connection(NULL), listen_thread(&db::listen, this) {
    connection = ensure_connection(connection);

    if (PQstatus(connection) == CONNECTION_OK) {
      PGresult   *res = PQexec(connection,
R"SQL(
BEGIN;
CREATE TABLE IF NOT EXISTS tracking_locations 
 (
   name      text not null primary key,
   x         real not null,
   y         real not null,
   z         real not null
 );

CREATE OR REPLACE FUNCTION tracking_location_notifier ()
 returns trigger
 language plpgsql
as $$
declare
  channel text := TG_ARGV[0];
begin
  PERFORM (
    select pg_notify(channel, NEW.name)
  );
  RETURN NULL;
end;
$$;

DROP TRIGGER IF EXISTS notify_tracking_locations ON tracking_locations;

CREATE TRIGGER notify_tracking_locations AFTER INSERT OR UPDATE
ON tracking_locations
FOR EACH ROW
EXECUTE PROCEDURE tracking_location_notifier('tracking_location_update');

COMMIT;
)SQL"

);
      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
          fprintf(stderr, "Command failed: %s", PQerrorMessage(connection));
      }

    }
  }

  db::~db() {
    PQfinish(connection);
    listen_thread.join();
  }

  void db::send(const std::string& name, const location& loc) {
    connection = ensure_connection(connection);
    if (PQstatus(connection) == CONNECTION_OK) {
      PGresult   *res;

      const char *paramValues[4];
      char paramValue[3][512];
      
      paramValues[0] = name.c_str();
      snprintf(paramValue[0], sizeof(paramValue[1]), "%f", loc.x);
      paramValues[1] = paramValue[0];
      snprintf(paramValue[1], sizeof(paramValue[2]), "%f", loc.y);
      paramValues[2] = paramValue[1];
      snprintf(paramValue[2], sizeof(paramValue[3]), "%f", loc.z);
      paramValues[3] = paramValue[2];


      res = PQexecParams(connection, 
        "INSERT into tracking_locations VALUES ($1, $2, $3, $4) ON CONFLICT (name) DO UPDATE SET x = EXCLUDED.x, y = EXCLUDED.y, z = EXCLUDED.z;",
        4, NULL, paramValues, NULL, NULL, 0);   

      if (PQresultStatus(res) != PGRES_COMMAND_OK)
      {
          fprintf(stderr, "INSERT command failed: %s", PQerrorMessage(connection));
      }
      PQclear(res);

    }
  }


  void db::listen() {
    PGconn     *conn = NULL;
    PGresult   *res = NULL;
    PGnotify   *notify = NULL;

    conn = ensure_connection(conn);
    res = PQexec(conn, "LISTEN tracking_location_update;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "LISTEN command failed: %s", PQerrorMessage(conn));
    }
    PQclear(res);


    while (true)
    {
      int         sock;
      fd_set      input_mask;

      sock = PQsocket(conn);

      if (sock < 0)
        break;              /* shouldn't happen */

      FD_ZERO(&input_mask);
      FD_SET(sock, &input_mask);

      if (select(sock + 1, &input_mask, NULL, NULL, NULL) < 0)
      {
        fprintf(stderr, "select() failed: %s\n", strerror(errno));
        conn = ensure_connection(conn);
      }

      /* Now check for input */
      PQconsumeInput(conn);
      while ((notify = PQnotifies(conn)) != NULL)
      {
        const char *paramValues[1];
        paramValues[0] = notify->extra;
        
        PGresult *select_result = PQexecParams(conn, "SELECT x, y, z from tracking_locations where name = $1", 1, NULL, paramValues, NULL, NULL, 0);   
        if (PQresultStatus(select_result) != PGRES_TUPLES_OK)
        {
            fprintf(stderr, "SELECT command failed: %s", PQerrorMessage(conn));
        } else {
          int num_rows = PQntuples(select_result);
          for(int i = 0;i < num_rows;i++) {
            float x = strtof(PQgetvalue(select_result, i, PQfnumber(select_result, "x")), NULL);
            float y = strtof(PQgetvalue(select_result, i, PQfnumber(select_result, "y")), NULL);
            float z = strtof(PQgetvalue(select_result, i, PQfnumber(select_result, "z")), NULL);
            
            receive(notify->extra, pixpq::location(x, y, z));
          }

        }

        PQfreemem(notify);
        PQconsumeInput(conn);
      }
    }
    PQfinish(conn);
  }


  PGconn * db::ensure_connection(PGconn *conn) {
    if (conn != NULL && PQstatus(conn) == CONNECTION_OK) {
      return conn;
    }
    PQfinish(conn);
    /* Make a connection to the database */
    conn = PQconnectdb(conninfo.c_str());

    /* Check to see that the backend connection was successfully made */
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s",
                PQerrorMessage(conn));
        return NULL;
    }

    PGresult   *res = PQexec(conn, "SET synchronous_commit TO OFF;");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Command failed: %s", PQerrorMessage(conn));
    }
    PQclear(res);
    return conn;
  }
    
}
