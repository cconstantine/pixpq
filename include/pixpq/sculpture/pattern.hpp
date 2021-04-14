#pragma once
#include <string>

namespace pixpq::sculpture {
  class pattern {
  public:
    pattern(const pqxx::row& r);
    pattern(const std::string& name, const std::string& glsl_code, bool enabled, bool overscan);

    static std::string notify_channel();
    static query by_id(const std::string& name);
    static query is_enabled();
    static query upsert(const pattern& p);

    std::string name;
    std::string glsl_code;
    bool enabled;
    bool overscan;
  };
}