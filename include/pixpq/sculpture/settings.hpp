#pragma once
#include <string>

#include <pixpq/manager.hpp>

namespace pixpq::sculpture {
  class settings {
  public:
    settings(const pqxx::row& row);

    std::string name;
    std::string active_pattern;
    float brightness, gamma;

    static std::string notify_channel();
    static query by_id(const std::string& name);
  
    static query get_or_create(const std::string& name, const std::string& active_pattern, float brightness, float gamma);
    static query update(const pixpq::sculpture::settings& s);
  };
}