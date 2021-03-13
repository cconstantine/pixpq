#pragma once
#include <string>
#include <vector>

#include <pixpq/sculpture/settings.hpp>

namespace pixpq::sculpture {

  class fadecandy {
  public:
    fadecandy(const pqxx::row& row);

    int id;
    std::string sculpture_name;
    std::string address;

    static query get_or_create(const pixpq::sculpture::settings& s, const std::string& address);
    static query get(const pixpq::sculpture::settings& s);
    static query update(const fadecandy& fc);
    static query remove(const fadecandy& fc);
  };

  class led {
  public:
    led(float x, float y, float z);    
    led(const pqxx::row& row);

    float x, y, z;

    static query find(const pixpq::sculpture::fadecandy &fc);
    static query upsert(const pixpq::sculpture::fadecandy &fc, const std::list<led>& leds);
  };
}
