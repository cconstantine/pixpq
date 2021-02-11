#pragma once
#include <string>
#include <pqxx/pqxx>

namespace pixpq::sculpture {
  class settings {
  public:
    static std::string get(const std::string& name);
    static std::string get_or_create(const std::string& name, const std::string& active_pattern, float brightness, float gamma);

    settings(const pqxx::row& row);

    std::string name;
    std::string active_pattern;
    float brightness, gamma;
  };
}