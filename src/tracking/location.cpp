#include <pixpq/manager.hpp>
#include <pixpq/notifier.hpp>

#include <pixpq/tracking/location.hpp>

namespace pixpq {
  namespace tracking {
    location::location(float x, float y, float z) : x(x), y(y), z(z) { }
  }

  template<>
  void manager::store(const std::string& name, const pixpq::tracking::location& loc) {
    std::lock_guard<std::mutex> m(connection_mutex);

    pqxx::work w(connection);

    w.exec(std::string("INSERT into tracking_locations (name, x, y, z) VALUES (") +
      w.quote(name) +
      ", " + w.quote(loc.x) +
      ", " + w.quote(loc.y) +
      ", " + w.quote(loc.z) + 
      ") ON CONFLICT (name) DO UPDATE SET x = EXCLUDED.x, y = EXCLUDED.y, z = EXCLUDED.z");
    w.commit();
  }

  template<>
  pixpq::tracking::location manager::get(const std::string& name) {
    std::lock_guard<std::mutex> m(connection_mutex);

    pqxx::work w(connection);
    pqxx::row r = w.exec1(std::string("SELECT x, y, z from tracking_locations where name = ") + w.quote(name) );

    return pixpq::tracking::location(r["x"].as<float>(), r["y"].as<float>(), r["z"].as<float>());
  }

  template<>
  void manager::set_listener(std::shared_ptr<listener<pixpq::tracking::location>> l) {
    std::lock_guard<std::mutex> m(connection_mutex);
    notifiers["tracking_location_update"] = std::make_shared<notifier<pixpq::tracking::location>>("tracking_location_update", this, l);
  }
}
