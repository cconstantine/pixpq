#include <pixpq/manager.hpp>

#include <pixpq/sculpture/pattern.hpp>

namespace pixpq {
  namespace sculpture {
    pattern::pattern(const std::string& glsl_code, bool enabled) : glsl_code(glsl_code), enabled(enabled) {}
  }

  // template<>
  // void manager::save(const std::string& name, const pixpq::sculpture::pattern& p) {
  //   std::lock_guard<std::mutex> m(connection_mutex);

  //   pqxx::work w(connection);

  //   w.exec(std::string("INSERT into patterns (name, glsl_code, enabled) VALUES (") +
  //     w.quote(name) +
  //     ", " + w.quote(p.glsl_code) +
  //     ", " + w.quote(p.enabled) +
  //     ") ON CONFLICT (name) DO UPDATE SET glsl_code = EXCLUDED.glsl_code, enabled = EXCLUDED.enabled");
  //   w.commit();
  // }

  // template<>
  // pixpq::sculpture::pattern manager::get<std::string, pixpq::sculpture::pattern>(const std::string& name) {
  //   std::lock_guard<std::mutex> m(connection_mutex);

  //   pqxx::work w(connection);
  //   pqxx::row r = w.exec1(std::string("SELECT glsl_code, enabled from patterns where name = ") + w.quote(name) );

  //   return pixpq::sculpture::pattern(r["glsl_code"].as<std::string>(), r["enabled"].as<bool>());
  // }

  // template<>
  // std::map<std::string, pixpq::sculpture::pattern> manager::get_all() {
  //   std::map<std::string, pixpq::sculpture::pattern> records;

  //   std::lock_guard<std::mutex> m(connection_mutex);
  //   pqxx::work w(connection);

  //   for(pqxx::row r : w.exec(std::string("SELECT name, glsl_code, enabled from patterns"))) {
  //     records.insert(std::map< std::string, pixpq::sculpture::pattern >::value_type(
  //       r["name"].as<std::string>(),
  //       pixpq::sculpture::pattern(r["glsl_code"].as<std::string>(), r["enabled"].as<bool>())
  //     ));
  //   }
  //   return records;
  // }

  // template<>
  // void manager::set_listener(std::shared_ptr<listener<pixpq::sculpture::pattern>> l) {
  //   notifiers["patterns_update"] = std::make_shared<notifier<pixpq::sculpture::pattern>>("patterns_update", this, l);
  // }
}
