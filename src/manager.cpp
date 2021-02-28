#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <pixpq/manager.hpp>

namespace pixpq {
  manager::manager(const std::string& opts) : connection(opts), notifier_connection(opts) {
    connection.set_variable("synchronous_commit", "off");
  }

  pqxx::connection& manager::get_notifier_connection() {
    return notifier_connection;
  }

  void manager::process_updates() {
    notifier_connection.get_notifs();
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

    /************** LEDs ************************/

  w.exec(R"SQL(
CREATE TABLE IF NOT EXISTS fadecandies
 (
   id               SERIAL PRIMARY KEY,
   sculpture_name   text REFERENCES sculpture_settings(name),
   address          text unique not null
 );
 )SQL");


  w.exec(R"SQL(
CREATE TABLE IF NOT EXISTS leds
 (
   fadecandy_id     integer REFERENCES fadecandies(id) NOT NULL,
   idx              integer NOT NULL,
   x                real NOT NULL,
   y                real NOT NULL,
   z                real NOT NULL,

   PRIMARY KEY (fadecandy_id, idx)
 );
 )SQL");


    w.commit();
  }

}
