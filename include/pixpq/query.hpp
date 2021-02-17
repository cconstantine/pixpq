#pragma once

#include <pqxx/pqxx>

namespace pixpq {
  class query {
  public:
    query(const std::string& name) :name(name) {}

    template<typename T>
    T exec(std::function<T(const std::string& , const std::string&)> f) const {
      return f("select * from tracking_locations where name = $1", name);
    }
  private:
    std::string name;
  };
}