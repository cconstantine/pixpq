#include <pixpq/tracking/location.hpp>

namespace pixpq::tracking {
  location::location(const std::string& name, float x, float y, float z) : name(name), x(x), y(y), z(z) { }
  location::location(const pqxx::row& r) :
   name(r["name"].as<std::string>()),
   x(r["x"].as<float>()),
   y(r["y"].as<float>()),
   z(r["z"].as<float>())
  { }

  std::string location::notify_channel() {
    return "tracking_location_update";
  }

  query location::by_id(const std::string& name) {
    return by_name(name);
  }

  query location::by_name(const std::string& name)  {
    return [=](pqxx::connection& c) -> pqxx::result {
      pqxx::work w(c);

      return w.exec_params_n(1,"select name, x, y, z from tracking_locations where name = $1", name);      
    };
  }

  query location::upsert(const location& l)  {
    return [=](pqxx::connection& c) -> pqxx::result {
      pqxx::work w(c);
      pqxx::result r = w.exec_params_n(1,
        "INSERT INTO tracking_locations(name, x, y, z) \
         VALUES ($1, $2, $3, $4) \
         ON CONFLICT (name) DO UPDATE set x = EXCLUDED.x, y = EXCLUDED.y, z = EXCLUDED.z \
         RETURNING name, x, y, z",
        l.name, l.x, l.y, l.z);
      w.commit();
      return r;
    };
  }
}
