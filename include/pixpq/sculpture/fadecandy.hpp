#pragma once
#include <string>
#include <vector>

#include <pqxx/pqxx>

#include <pixpq/sculpture/settings.hpp>

namespace pixpq::sculpture {

  class led {
    public:
    led(const pqxx::row& row);

    int fadecandy_id;
    float x, y, z;
  };

  class fadecandy {
  public:
    fadecandy(const pixpq::sculpture::settings& s, const std::string& address);
    fadecandy(const pqxx::row& row);

    int id;
    std::string sculpture_name;
    std::string address;

    static query<std::string> by_sculpture(const pixpq::sculpture::settings& s);
    static query<std::string, std::string, int> update(const fadecandy& fc);
    static query<std::string, std::string> insert(const fadecandy& fc);
    static query<int> remove(const fadecandy& fc);
  };
}