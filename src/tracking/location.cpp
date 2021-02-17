#include <pixpq/manager.hpp>
#include <pixpq/notifier.hpp>

#include <pixpq/tracking/location.hpp>

namespace pixpq::tracking {
  location::location(const std::string& name, float x, float y, float z) : name(name), x(x), y(y), z(z) { }
  location::location(const pqxx::row& r) :
   name(r["name"].as<std::string>()),
   x(r["x"].as<float>()),
   y(r["y"].as<float>()),
   z(r["z"].as<float>())
  { }

  void location::save(pqxx::connection& connection) {
    pqxx::work w(connection);

    w.exec(std::string("INSERT into tracking_locations (name, x, y, z) VALUES (") +
      w.quote(name) +
      ", " + w.quote(x) +
      ", " + w.quote(y) +
      ", " + w.quote(z) + 
      ") ON CONFLICT (name) DO UPDATE SET x = EXCLUDED.x, y = EXCLUDED.y, z = EXCLUDED.z");
    w.commit();
  }

}
namespace pixpq {
  // template<>
  // void manager::save(const std::string& name, const pixpq::tracking::location& loc) {
  //   std::lock_guard<std::mutex> m(connection_mutex);

  //   pqxx::work w(connection);

  //   w.exec(std::string("INSERT into tracking_locations (name, x, y, z) VALUES (") +
  //     w.quote(name) +
  //     ", " + w.quote(loc.x) +
  //     ", " + w.quote(loc.y) +
  //     ", " + w.quote(loc.z) + 
  //     ") ON CONFLICT (name) DO UPDATE SET x = EXCLUDED.x, y = EXCLUDED.y, z = EXCLUDED.z");
  //   w.commit();
  // }

  template<>
  pixpq::tracking::location manager::get(const std::string& name) {
    std::lock_guard<std::mutex> m(connection_mutex);

    pqxx::work w(connection);
    pqxx::row r = w.exec1(std::string("SELECT name, x, y, z from tracking_locations where name = ") + w.quote(name) );

    return pixpq::tracking::location(r["name"].as<std::string>(), r["x"].as<float>(), r["y"].as<float>(), r["z"].as<float>());
  }

  template<>
  std::map<std::string, pixpq::tracking::location> manager::get_all() {
    std::map<std::string, pixpq::tracking::location> records;

    std::lock_guard<std::mutex> m(connection_mutex);
    pqxx::work w(connection);

    for(pqxx::row r : w.exec(std::string("SELECT name, x, y, z from tracking_locations"))) {
      records.insert(std::map< std::string, pixpq::tracking::location >::value_type(
        r["name"].as<std::string>(),
        pixpq::tracking::location(r["name"].as<std::string>(), r["x"].as<float>(), r["y"].as<float>(), r["z"].as<float>())
      ));
    }
    return records;
  }

  template<>
  void manager::set_listener(std::shared_ptr<listener<pixpq::tracking::location>> l) {
    std::lock_guard<std::mutex> m(connection_mutex);
    notifiers["tracking_location_update"] = std::make_shared<notifier<pixpq::tracking::location>>("tracking_location_update", this, l);
  }
}
