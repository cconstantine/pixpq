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
    virtual void update(const std::string& name,const T& t) = 0;
  };

  template<typename T> class notifier;

  class manager {
  public:
    manager( const std::string& opts);

    void ensure_schema();

    template<typename T>
    void set_listener(std::shared_ptr<listener<T>> l){
      notifiers[T::notify_channel()] = std::make_shared<notifier<T>>(T::notify_channel(), this, l);
    }

    template<typename T, typename Q>
    T get(const Q& q) {
      std::cout << "get" << std::endl;
      pqxx::work w(connection);
      pqxx::result result = q(
          [&](const std::string& q, auto... params) -> pqxx::result {
            return w.exec_params_n(1, q, params...);
          }
        );
      T obj(result.front());

      w.commit();

      return obj;
    }

    template<typename T, typename Q>
    std::vector<T> get_all(const Q& q) {
      pqxx::work w(connection);
      
      pqxx::result result = q(
        [&](const std::string& q, auto... params) -> pqxx::result {
          return w.exec_params(q, params...);
        }
      );
      std::vector<T> records;
      records.reserve(result.size());

      for(pqxx::row r : result) {
        records.push_back(T(r));
      }

      w.commit();
      return records;
    }

    pqxx::connection& get_notifier_connection();

    void process_updates();

  private:
    pqxx::connection connection;
    pqxx::connection notifier_connection;

    std::mutex connection_mutex;
    std::map<std::string, std::shared_ptr<pqxx::notification_receiver>> notifiers;
  };

  template<typename... T>
  using querier = typename std::function<pqxx::result(const std::string& , const T&...)>;

  template<typename... T>
  using query = typename std::function<pqxx::result(querier<T...> )>;

  template<typename T>
  class notifier : public pqxx::notification_receiver {
  public:
    notifier(const std::string& channel, manager* mgr, std::shared_ptr<listener<T>> l) :
     pqxx::notification_receiver(mgr->get_notifier_connection(), channel),
     mgr(mgr), l(l) {  }

    void operator() (const std::string& payload, int backend_pid){
      l->update(payload, mgr->get<T>(T::by_id(payload)));
    }

  private:
    manager* mgr;
    std::shared_ptr<listener<T>> l;
  };

}
