#include <pixpq/manager.hpp>
#include <pixpq/notifier.hpp>

#include <pixpq/sculpture/settings.hpp>

namespace pixpq {
  namespace sculpture {
    std::string settings::get_or_create(const std::string& name, const std::string& active_pattern, float brightness, float gamma) {
      return "";
    }
    
    settings::settings(const pqxx::row& r) :
     active_pattern(r["active_pattern"].as<std::string>()),
     brightness(r["brightness"].as<float>()),
     gamma(r["gamma"].as<float>())
    { }
  }

  template<>
  void manager::save(const pixpq::sculpture::settings& s) {
    // std::lock_guard<std::mutex> m(connection_mutex);
    // pqxx::work w(connection);

    // w.exec(std::string("INSERT into sculpture_settings (name, active_pattern, brightness, gamma) VALUES (") +
    //   w.quote(name) +
    //   ", " + w.quote(s.active_pattern) +
    //   ", " + w.quote(s.brightness) +
    //   ", " + w.quote(s.gamma) +
    //   ") ON CONFLICT (name) DO UPDATE SET active_pattern = EXCLUDED.active_pattern, brightness = EXCLUDED.brightness, gamma = EXCLUDED.gamma");
    // w.commit();
  }

  template<>
  pixpq::sculpture::settings manager::get<std::string, pixpq::sculpture::settings>(const std::string& name) {
    std::lock_guard<std::mutex> m(connection_mutex);

    pqxx::work w(connection);
    pqxx::row r = w.exec1(std::string("SELECT active_pattern, brightness, gamma from sculpture_settings where name = ") + w.quote(name) );

    return pixpq::sculpture::settings(r);
  }

  template<>
  std::map<std::string, pixpq::sculpture::settings> manager::get_all() {
    std::map<std::string, pixpq::sculpture::settings> records;

    std::lock_guard<std::mutex> m(connection_mutex);
    pqxx::work w(connection);

    for(pqxx::row r : w.exec(std::string("SELECT name, active_pattern, brightness, gamma from sculpture_settings"))) {
      records.insert(std::map< std::string, pixpq::sculpture::settings >::value_type(
        r["name"].as<std::string>(),
        pixpq::sculpture::settings(r)
      ));
    }
    return records;
  }


  template<>
  void manager::set_listener(std::shared_ptr<listener<pixpq::sculpture::settings>> l) {
    notifiers["sculpture_settings_update"] = std::make_shared<notifier<pixpq::sculpture::settings>>("sculpture_settings_update", this, l);
  }
}
