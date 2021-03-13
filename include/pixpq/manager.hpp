#pragma once
#include <pqxx/pqxx>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

namespace pixpq {
  class manager;

  template<typename T>
  class listener {
  public:
    virtual void update(const T& t) = 0;
  };

  template<typename T> class notifier;
  using query = std::function<pqxx::result(pqxx::connection&)>;

  class manager {
  public:
    manager( const std::string& opts);

    void ensure_schema();

    template<typename T>
    void set_listener(std::shared_ptr<listener<T>> l){
      notifiers[T::notify_channel()] = std::make_shared<notifier<T>>(T::notify_channel(), this, l);
    }

    template<typename T>
    T get(query f) {
      T obj(f(connection).front());
      return obj;
    }

    template<typename T>
    std::vector<T> get_all(std::function<pqxx::result(pqxx::connection&)> f) {
     
      pqxx::result result = f(connection);
      std::vector<T> records;
      records.reserve(result.size());

      for(pqxx::row r : result) {
        records.push_back(T(r));
      }

      return records;
    }

    pqxx::connection& get_notifier_connection();

    void process_updates();

  private:
    pqxx::connection connection;

    std::map<std::string, std::shared_ptr<pqxx::notification_receiver>> notifiers;
  };

  template<typename... T>
  using querier = typename std::function<pqxx::result(const std::string& , const T&...)>;


  template<typename T>
  class notifier : public pqxx::notification_receiver {
  public:
    notifier(const std::string& channel, manager* mgr, std::shared_ptr<listener<T>> l) :
     pqxx::notification_receiver(mgr->get_notifier_connection(), channel),
     mgr(mgr), l(l) {  }

    void operator() (const std::string& payload, int backend_pid){
      l->update(mgr->get<T>(T::by_id(payload)));
    }

  private:
    manager* mgr;
    std::shared_ptr<listener<T>> l;
  };

}
