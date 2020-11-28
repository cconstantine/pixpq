#include <pixpq/manager.hpp>
#include <pixpq/notifier.hpp>

#include <pixpq/sculpture/settings.hpp>

namespace pixpq {
  namespace sculpture {
    settings::settings(const std::string& active_pattern, float brightness, float gamma) : active_pattern(active_pattern), brightness(brightness), gamma(gamma) {}
  }

  template<>
  void manager::store(const std::string& name, const pixpq::sculpture::settings& s) {
    std::lock_guard<std::mutex> m(connection_mutex);
    pqxx::work w(connection);

    w.exec(std::string("INSERT into sculpture_settings (name, active_pattern, brightness, gamma) VALUES (") +
      w.quote(name) +
      ", " + w.quote(s.active_pattern) +
      ", " + w.quote(s.brightness) +
      ", " + w.quote(s.gamma) +
      ") ON CONFLICT (name) DO UPDATE SET active_pattern = EXCLUDED.active_pattern, brightness = EXCLUDED.brightness, gamma = EXCLUDED.gamma");
    w.commit();
  }

  template<>
  pixpq::sculpture::settings manager::get<pixpq::sculpture::settings>(const std::string& name) {
    std::lock_guard<std::mutex> m(connection_mutex);

    pqxx::work w(connection);
    pqxx::row r = w.exec1(std::string("SELECT active_pattern, brightness, gamma from sculpture_settings where name = ") + w.quote(name) );

    return pixpq::sculpture::settings(r["active_pattern"].as<std::string>(), r["brightness"].as<float>(), r["gamma"].as<float>());
  }

  template<>
  void manager::set_listener(std::shared_ptr<listener<pixpq::sculpture::settings>> l) {
    notifiers["sculpture_settings_update"] = std::make_shared<notifier<pixpq::sculpture::settings>>("sculpture_settings_update", this, l);
  }
}
