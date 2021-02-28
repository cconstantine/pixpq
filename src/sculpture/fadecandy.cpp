#include <pixpq/manager.hpp>

#include <pixpq/sculpture/fadecandy.hpp>
#include <pixpq/sculpture/settings.hpp>

namespace pixpq {
  namespace sculpture {
    int id;
    std::string sculpture_name;
    std::string address;

    fadecandy::fadecandy(const pixpq::sculpture::settings& s, const std::string& address) :
     id(-1), sculpture_name(s.name), address(address)
    { }

    fadecandy::fadecandy(const pqxx::row& row) : 
     id(row["id"].as<int>()), sculpture_name(row["sculpture_name"].as<std::string>()), address(row["address"].as<std::string>())
    { } 

    query<std::string> fadecandy::by_sculpture(const pixpq::sculpture::settings& s) {
      return [=](querier<std::string> f) -> pqxx::result {
        return f("SELECT id, sculpture_name, address FROM fadecandies WHERE sculpture_name = $1", s.name);
      };
    }

    query<std::string, std::string, int> fadecandy::update(const fadecandy& fc) {
      return [=](querier<std::string, std::string, int> f) -> pqxx::result {
        return f("UPDATE fadecandies \
                  SET sculpture_name = $1, address = $2 \
                  WHERE id = $3 \
                  RETURNING id, sculpture_name, address",
                 fc.sculpture_name, fc.address, fc.id);
      };
    }

    query<std::string, std::string> fadecandy::insert(const fadecandy& fc) {
      return [=](querier<std::string, std::string> f) -> pqxx::result {
        return f("INSERT INTO fadecandies(sculpture_name, address) \
                  VALUES($1, $2) \
                  RETURNING id, sculpture_name, address",
                 fc.sculpture_name, fc.address);
      };
    }

  }
}
