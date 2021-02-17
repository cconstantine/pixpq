#pragma once
#include <string>

#include <pixpq/record.hpp>

namespace pixpq::tracking {

  class location : public pixpq::record {
  public:
    location(const pqxx::row& r);
    location(const std::string& name, float x, float y, float z);

    virtual void save(pqxx::connection& c);

    std::string name;
    float x, y, z;

  };
}