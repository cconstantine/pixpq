#include <pixpq/manager.hpp>

#include <pixpq/sculpture/pattern.hpp>

namespace pixpq {
  namespace sculpture {
    pattern::pattern(const std::string& name, const std::string& glsl_code, bool enabled, bool overscan) : 
     name(name), glsl_code(glsl_code), enabled(enabled), overscan(overscan)
    { }

    pattern::pattern(const pqxx::row& r) : 
     name(r["name"].as<std::string>()), glsl_code(r["glsl_code"].as<std::string>()), enabled(r["enabled"].as<bool>()), overscan(r["overscan"].as<bool>())
    { }

    std::string pattern::notify_channel() {
      return "patterns_update";
    }

    query pattern::by_id(const std::string& name) {
      return [=](pqxx::connection& c) -> pqxx::result {
        pqxx::work w(c);

        return w.exec_params_n(1,
          "SELECT name, glsl_code, enabled, overscan \
           FROM patterns \
           WHERE name = $1", name);
      };
    }

    query pattern::is_enabled() {
      return [=](pqxx::connection& c) -> pqxx::result {
        pqxx::work w(c);
        return w.exec_params(
          "SELECT name, glsl_code, enabled, overscan \
           FROM patterns \
           WHERE enabled = $1",
           true);
      };
    }

    query pattern::upsert(const pattern& p) {
      return [=](pqxx::connection& c) -> pqxx::result {
        pqxx::work w(c);

        pqxx::result r = w.exec_params_n(1,
          "INSERT INTO patterns (name, glsl_code, enabled, overscan) \
           VALUES ($1, $2, $3, $4) \
           ON CONFLICT (name) DO UPDATE SET glsl_code = EXCLUDED.glsl_code, enabled = EXCLUDED.enabled, overscan = EXCLUDED.overscan \
           RETURNING name, glsl_code, enabled, overscan",
           p.name, p.glsl_code, p.enabled, p.overscan);
        w.commit();
        return r;
      };
    }
  }
}
