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

  query<std::string> location::by_id(const std::string& name) {
    return by_name(name);
  }

  query<std::string> location::by_name(const std::string& name)  {
    return [=](querier<std::string> f) -> pqxx::result {
      return f("select name, x, y, z from tracking_locations where name = $1", name);
    };
  }

  query<std::string, float, float, float> location::upsert(const location& l)  {
    return [=](querier<std::string, float, float, float> f) -> pqxx::result {
      return f(
        "INSERT INTO tracking_locations(name, x, y, z) \
         VALUES ($1, $2, $3, $4) \
         ON CONFLICT (name) DO UPDATE set x = EXCLUDED.x, y = EXCLUDED.y, z = EXCLUDED.z \
         RETURNING name, x, y, z",
        l.name, l.x, l.y, l.z);
    };
  }
}
