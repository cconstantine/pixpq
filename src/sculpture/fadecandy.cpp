#include <pixpq/manager.hpp>

#include <pixpq/sculpture/fadecandy.hpp>
#include <pixpq/sculpture/settings.hpp>

namespace pixpq {
  namespace sculpture {
    int id;
    std::string sculpture_name;
    std::string address;

    fadecandy::fadecandy(const pqxx::row& row) : 
     id(row["id"].as<int>()), sculpture_name(row["sculpture_name"].as<std::string>()), address(row["address"].as<std::string>())
    { } 

    query fadecandy::get_or_create(const pixpq::sculpture::settings& s, const std::string& address) {
      return [=](pqxx::connection& c) -> pqxx::result {
        pqxx::work w(c);
        pqxx::result r = w.exec_params_n(1, 
          "INSERT INTO fadecandies(sculpture_name, address) \
           VALUES($1, $2) \
           ON CONFLICT(address) DO UPDATE set sculpture_name = EXCLUDED.sculpture_name \
           RETURNING id, sculpture_name, address",
           s.name, address);
        w.commit();
        return r;
      };
    }

    query fadecandy::get(const pixpq::sculpture::settings& s) {
      return [=](pqxx::connection& c) -> pqxx::result {
        pqxx::work w(c);
        return w.exec_params( 
          "SELECT id, sculpture_name, address \
           FROM fadecandies \
           WHERE sculpture_name = $1",
           s.name);
      };
    }

    query fadecandy::update(const fadecandy& fc) {
      return [=](pqxx::connection& c) -> pqxx::result {
        pqxx::work w(c);
        return w.exec_params_n(1,
          "UPDATE fadecandies \
           SET (sculpture_name, address) \
           VALUES ($1, $2) \
           WHERE id=$3 \
           RETURNING id, sculpture_name, address",
           fc.sculpture_name, fc.address);
      };
    }

    query fadecandy::remove(const fadecandy& fc) {
      return [=](pqxx::connection& c) -> pqxx::result {
        pqxx::work w(c);
        return w.exec_params_n(0,
          "DELETE FROM fadecandies \
           WHERE id = $1",
           fc.id);
      };
    }

    led::led(float x, float y, float z) : x(x), y(y), z(z)
    { }

    led::led(const pqxx::row& row) : x(row["x"].as<float>()), y(row["y"].as<float>()), z(row["z"].as<float>())
    { }

    query led::find(const pixpq::sculpture::fadecandy &fc) {
      return [=](pqxx::connection& c) -> pqxx::result {
        pqxx::work w(c);
        return w.exec_params(
          "SELECT x, y, z \
           FROM leds \
           WHERE fadecandy_id = $1 \
           ORDER BY idx",
          fc.id);
      };
    }

    query led::upsert(const pixpq::sculpture::fadecandy &fc, const std::list<led>& leds) {
      return [=](pqxx::connection& c) -> pqxx::result {
        pqxx::work w(c);
        w.exec_params(
          "DELETE FROM leds \
           WHERE fadecandy_id = $1 AND idx >= $2 ",
           fc.id, leds.size());

        int idx = 0;
        for(const led& led : leds) {
          w.exec_params(
            "INSERT INTO leds (fadecandy_id, idx, x, y, z) \
             VALUES ($1, $2, $3, $4, $5) \
             ON CONFLICT (fadecandy_id, idx) DO UPDATE set x = EXCLUDED.x, y = EXCLUDED.y, z = EXCLUDED.z ",
             fc.id, idx++, led.x, led.y, led.z
            );
        }
        w.commit();
        return pqxx::result();
      };
    }
  }
}
