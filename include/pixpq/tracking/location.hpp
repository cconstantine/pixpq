#pragma once
#include <string>

#include <pixpq/manager.hpp>

namespace pixpq::tracking {

  class location {
  public:
    location(const pqxx::row& r);
    location(const std::string& name, float x, float y, float z);

    std::string name;
    float x, y, z;

    static std::string notify_channel();
    static query<std::string> by_id(const std::string& name);

    static query<std::string> by_name(const std::string& name);
    static query<std::string, float, float, float> upsert(const location& l);
  };
}
