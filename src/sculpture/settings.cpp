#include <pixpq/manager.hpp>

#include <pixpq/sculpture/settings.hpp>

namespace pixpq {
  namespace sculpture {
    
    settings::settings(const pqxx::row& r) :
     name(r["name"].as<std::string>()),
     active_pattern(r["active_pattern"].as<std::string>()),
     brightness(r["brightness"].as<float>()),
     gamma(r["gamma"].as<float>())
    { }

    std::string settings::notify_channel() {
      return "sculpture_settings_update";
    }

    query settings::by_id(const std::string& name) {
      return [=](pqxx::connection& c) -> pqxx::result {
        pqxx::work w(c);

        return w.exec_params_n(1,
          "SELECT name, active_pattern, brightness, gamma \
           FROM sculpture_settings \
           WHERE name = $1", name);
      };
    }

    query settings::get_or_create(const std::string& name, const std::string& active_pattern, float brightness, float gamma) {
      return [=](pqxx::connection& c) -> pqxx::result {
        pqxx::work w(c);

        pqxx::result r = w.exec_params_n(1,
          "INSERT into sculpture_settings (name, active_pattern, brightness, gamma) \
           VALUES ($1, $2, $3, $4) \
           ON CONFLICT (name) DO UPDATE SET name = EXCLUDED.name \
           RETURNING name, active_pattern, brightness, gamma",
           name, active_pattern, brightness, gamma);
        w.commit();
        return r;
      };
    }

    query settings::update(const pixpq::sculpture::settings& s) {
      return [=](pqxx::connection& c) -> pqxx::result {
        pqxx::work w(c);

        pqxx::result r = w.exec_params_n(1,
          "UPDATE sculpture_settings \
           SET active_pattern = $1, brightness = $2, gamma = $3 \
           WHERE name = $4 \
           RETURNING name, active_pattern, brightness, gamma",
           s.active_pattern, s.brightness, s.gamma, s.name);
        w.commit();
        return r;
      };
    }
  }
}
