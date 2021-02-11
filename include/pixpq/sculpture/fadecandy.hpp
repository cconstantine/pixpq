#pragma once
#include <string>
#include <vector>

#include <pqxx/pqxx>

namespace pixpq::sculpture {

  class led {
    public:
    led(const pqxx::row& row);

    int fadecandy_id;
    float x, y, z;
  };

  class fadecandy {
  public:
    static std::string get_or_create(const std::string& sculpture_name);
    fadecandy(const pqxx::row& row);

    int id;
    std::string sculpture_name;
    std::string address;
  };
}