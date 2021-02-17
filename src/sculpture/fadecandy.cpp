#include <pixpq/manager.hpp>

#include <pixpq/sculpture/fadecandy.hpp>
#include <pixpq/sculpture/settings.hpp>

namespace pixpq {
  namespace sculpture {
    // fadecandy::fadecandy(const std::string& address) : id(-1), address(address) { }
    // fadecandy::fadecandy(int id, const std::string& address) : id(id), address(address) { }
    // led::led(float x, float y, float z) : x(x), y(y), z(z) { }
  }

  // template<>
  // int manager::save( pixpq::sculpture::fadecandy& fc) {
  //   int id = fc.id;
  //   std::lock_guard<std::mutex> m(connection_mutex);
  //   pqxx::work w(connection);

  //   return w.exec1(std::string("INSERT into fadecandies (sculpture_name, address) VALUES (") +
  //      w.quote(fc.sculpture_name) + ", " + w.quote(fc.address) +
  //     ") RETURNING id")["id"].as<int>();
  // }

  // template<>
  // void manager::save(const int& id, const pixpq::sculpture::fadecandy& fc) {
  //   std::lock_guard<std::mutex> m(connection_mutex);
  //   pqxx::work w(connection);
  //   w.exec(std::string("UPDATE fadecandies SET (sculpture_name, address) = (") +
  //      w.quote(fc.sculpture_name) + ", " + w.quote(fc.address) +
  //      ") WHERE id = " + w.quote(fc.id));
  // }

  //   w.exec(std::string("DELETE FROM leds where fadecandy_id = " + w.quote(fc.id) + " AND idx >= " + w.quote(fc.leds.size())));

  //   for(size_t i = 0;i < fc.leds.size();i++) {
  //     w.exec(std::string("INSERT into leds (fadecandy_id, idx, x, y, z) VALUES (") + 
  //       w.quote(fc.id) + 
  //       ", " + w.quote(i) +
  //       ", " + w.quote(fc.leds[i].x) + ", " + w.quote(fc.leds[i].y) + ", " + w.quote(fc.leds[i].z) +
  //       ") ON CONFLICT (fadecandy_id, idx) DO UPDATE SET x = EXCLUDED.x, y = EXCLUDED.y, z = EXCLUDED.z");
  //   }

  //   w.commit();
  //   return id;
  // }

  template<>
  pixpq::sculpture::fadecandy manager::get<int, pixpq::sculpture::fadecandy>(const int& id) {
    std::lock_guard<std::mutex> m(connection_mutex);

    pqxx::work w(connection);
    pqxx::row r = w.exec1(std::string("SELECT address from fadecandies where id = ") + w.quote(id) );
    return pixpq::sculpture::fadecandy(r);
  }

  // template<>
  // std::vector<pixpq::sculpture::fadecandy> manager::get<std::vector<pixpq::sculpture::fadecandy>>(const pixpq::sculpture::settings& s) {
  //   std::lock_guard<std::mutex> m(connection_mutex);

  //   pqxx::work w(connection);
  //   pqxx::row r = w.exec1(std::string("SELECT active_pattern, brightness, gamma from sculpture_settings where name = ") + w.quote(name) );

  //   return pixpq::sculpture::settings(r["active_pattern"].as<std::string>(), r["brightness"].as<float>(), r["gamma"].as<float>());
  // }

  template<>
  std::map<std::string, std::vector<pixpq::sculpture::fadecandy>> manager::get_all() {
    std::map<std::string, std::vector<pixpq::sculpture::fadecandy>> records;

    std::lock_guard<std::mutex> m(connection_mutex);
    pqxx::work w(connection);

    for(pqxx::row r : w.exec(std::string("SELECT id, address, sculpture_name from sculpture_settings"))) {
      records[r["sculpture_name"].as<std::string>()].push_back(pixpq::sculpture::fadecandy(r));
    }
    return records;
  }


}
