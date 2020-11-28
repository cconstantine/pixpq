#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <pixpq/manager.hpp>
#include <pixpq/notifier.hpp>

namespace pixpq {
  manager::manager(const std::string& opts) : connection(opts), notifier_connection(opts), active(true), listening_thread(&manager::listen_method, this) {
    connection.set_variable("synchronous_commit", "off");
  }

  manager::~manager() {
    active = false;
    listen_latch.notify_all();

    notifier_connection.close();
    listening_thread.join();
  }

  void manager::listen_method() {
    std::unique_lock<std::mutex> lk(listen_mutex);
    listen_latch.wait(lk);

    fprintf(stderr, "active: %d\n", active);
    while (active) {
      try {
        notifier_connection.await_notification();
      } catch (const pqxx::broken_connection& e) { /* ignore broken connections */ }
    }
  }

  pqxx::connection& manager::get_notifier_connection() {
    return notifier_connection;
  }

  void manager::start_listening() {
    listen_latch.notify_all();
  }

  void manager::ensure_schema() {
    pqxx::work w(connection);

    /************** tracking_locations **************/
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
    /************************************************/

    /************** sculpture_settings **************/
    w.exec(R"SQL(
CREATE TABLE IF NOT EXISTS sculpture_settings
 (
   name             text not null primary key,
   active_pattern   text not null,
   brightness       real not null,
   gamma            real not null
 );
 )SQL");

    w.exec(R"SQL(
CREATE OR REPLACE FUNCTION sculpture_settings_notifier ()
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
DROP TRIGGER IF EXISTS notify_sculpture_settings ON sculpture_settings;
  )SQL");

    w.exec(R"SQL(
CREATE TRIGGER notify_sculpture_settings AFTER INSERT OR UPDATE
ON sculpture_settings
FOR EACH ROW
EXECUTE PROCEDURE sculpture_settings_notifier('sculpture_settings_update');
  )SQL");


    /************** patterns ************************/
      w.exec(R"SQL(
CREATE TABLE IF NOT EXISTS patterns
 (
   name             text not null primary key,
   glsl_code        text not null,
   enabled          boolean not null
 );
 )SQL");

    w.exec(R"SQL(
CREATE OR REPLACE FUNCTION patterns_notifier ()
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
DROP TRIGGER IF EXISTS notify_patterns ON patterns;
  )SQL");

    w.exec(R"SQL(
CREATE TRIGGER notify_patterns AFTER INSERT OR UPDATE
ON patterns
FOR EACH ROW
EXECUTE PROCEDURE patterns_notifier('patterns_update');
  )SQL");
    /************************************************/

    w.commit();
  }

}
