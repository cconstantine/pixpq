#include <pixpq/pixpq.hh>

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

namespace pixpq::tracking {

  location::location(float x, float y, float z) : x(x), y(y), z(z) { }

  notifier::notifier(manager* mgr, std::shared_ptr<listener> l) :
   pqxx::notification_receiver(mgr->get_notifier_connection(), "tracking_location_update"),
   active(true), mgr(mgr), l(l), listening_thread(&notifier::listen_method, this) {  }

  notifier::~notifier() {
    active = false;
    listening_thread.join();
  }
  void notifier::operator() (const std::string& payload, int backend_pid) {
    l->update(payload, mgr->get(payload));
  }

  void notifier::listen_method() {
    while (active) {
      mgr->get_notifier_connection().await_notification();
    }
  }

  
  manager::manager(const std::string& opts) : connection(opts) {
    connection.set_variable("synchronous_commit", "off");
  }

  void manager::set_listener(std::shared_ptr<pixpq::tracking::listener> l) {
    n = std::make_shared<notifier>(this, l);
  }

  void manager::ensure_schema() {
    pqxx::work w(connection);

    w.exec(R"SQL(
CREATE TABLE IF NOT EXISTS tracking_locations
 (
   name      text not null primary key,
   x         real not null,
   y         real not null,
   z         real not null
 );
 )SQL");

    w.exec(R"SQL(
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
  )SQL");

    w.exec(R"SQL(
DROP TRIGGER IF EXISTS notify_tracking_locations ON tracking_locations;
  )SQL");

    w.exec(R"SQL(
CREATE TRIGGER notify_tracking_locations AFTER INSERT OR UPDATE
ON tracking_locations
FOR EACH ROW
EXECUTE PROCEDURE tracking_location_notifier('tracking_location_update');
  )SQL");
    w.commit();
  }


  void manager::store(const std::string& name, const location& loc) {
    pqxx::work w(connection);

    w.exec(std::string("INSERT into tracking_locations (name, x, y, z) VALUES (") +
      w.quote(name) +
      ", " + w.quote(loc.x) +
      ", " + w.quote(loc.y) +
      ", " + w.quote(loc.z) + 
      ") ON CONFLICT (name) DO UPDATE SET x = EXCLUDED.x, y = EXCLUDED.y, z = EXCLUDED.z");
    w.commit();
  }


  pixpq::tracking::location manager::get(const std::string& name) {
    pqxx::work w(connection);
    pqxx::row r = w.exec1(std::string("SELECT x, y, z from tracking_locations where name = ") + w.quote(name) );

    return pixpq::tracking::location(r["x"].as<float>(), r["z"].as<float>(), r["z"].as<float>());
  }

  pqxx::connection& manager::get_notifier_connection() {
    return notifier_connection;
  }
}
