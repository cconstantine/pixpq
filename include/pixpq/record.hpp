#pragma once
#include <pqxx/pqxx>

namespace pixpq {
  class record {
  public:
    virtual void save(pqxx::connection& c) = 0;
  };
}