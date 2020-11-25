#pragma once
#include <pqxx/pqxx>
#include <thread>

namespace pixpq::sculpture {
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

    template<typename T>
    void store(const std::string& name, const T& loc);
    
    template<typename T>
    T get(const std::string& name);

    pqxx::connection& get_notifier_connection();
  private:
    pqxx::connection connection;
    std::map<std::string, std::shared_ptr<pqxx::notification_receiver>> notifiers;
    pqxx::connection notifier_connection;
  };

  template<typename T>
  class notifier : public pqxx::notification_receiver {
  public:
    notifier(const std::string& channel, manager* mgr, std::shared_ptr<listener<T>> l);
    ~notifier();

    void operator() (const std::string& payload, int backend_pid);

  private:
    bool active;

    void listen_method();
    manager* mgr;
    std::shared_ptr<listener<T>> l;
    std::thread listening_thread;
  };
}