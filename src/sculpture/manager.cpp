#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <pixpq/sculpture/manager.hh>

namespace pixpq::sculpture {
  manager::manager(const std::string& opts) : connection(opts) {
    connection.set_variable("synchronous_commit", "off");
  }

  void manager::set_listener(std::shared_ptr<pixpq::sculpture::listener> l) {
    n = std::make_shared<notifier>(this, l);
  }

  void manager::ensure_schema() {
    pqxx::work w(connection);

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
    w.commit();
  }

  void manager::store(const std::string& name, const pixpq::sculpture::settings& s) {
    pqxx::work w(connection);

    w.exec(std::string("INSERT into sculpture_settings (name, active_pattern, brightness, gamma) VALUES (") +
      w.quote(name) +
      ", " + w.quote(s.active_pattern) +
      ", " + w.quote(s.brightness) +
      ", " + w.quote(s.gamma) +
      ") ON CONFLICT (name) DO UPDATE SET active_pattern = EXCLUDED.active_pattern, brightness = EXCLUDED.brightness, gamma = EXCLUDED.gamma");
    w.commit();
  }

  pixpq::sculpture::settings manager::get(const std::string& name) {
    pqxx::work w(connection);
    pqxx::row r = w.exec1(std::string("SELECT active_pattern, brightness, gamma from sculpture_settings where name = ") + w.quote(name) );

    return pixpq::sculpture::settings(r["active_pattern"].as<std::string>(), r["brightness"].as<float>(), r["gamma"].as<float>());
  }

  pqxx::connection& manager::get_notifier_connection() {
    return notifier_connection;
  }
}
