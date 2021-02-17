#pragma once
#include <pqxx/pqxx>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <pixpq/record.hpp>
#include <pixpq/query.hpp>

namespace pixpq {
  template<typename T>
  class listener {
  public:
    virtual void update(const std::string& name,const T& t) = 0;
  };

  class manager {
  public:
    manager( const std::string& opts);

    void ensure_schema();

    template<typename T>
    void set_listener(std::shared_ptr<listener<T>> l);

    void save(pixpq::record& obj);

    template<typename T>
    T get(const pixpq::query& q) {
      pqxx::work w(connection);
      return T(
        q.exec<pqxx::row>(
          [&](const std::string& q, auto... params) -> pqxx::row { return w.exec_params1(q, params...); }
        )
      );
    }

    // template<typename K, typename T>
    // void save(const K& id, const T& obj);
    
    template<typename K, typename T>
    T get(const K& id);

    template<typename K, typename T>
    std::map<K, T> get_all();

    template<typename K, typename T, typename R>
    std::map<K, T> get_refs(const R& reference);

    pqxx::connection& get_notifier_connection();

    void process_updates();

  private:
    pqxx::connection connection;
    pqxx::connection notifier_connection;

    std::mutex connection_mutex;
    std::map<std::string, std::shared_ptr<pqxx::notification_receiver>> notifiers;
  };

}
