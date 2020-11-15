#pragma once
#include <pqxx/pqxx>
#include <thread>

#include <pixpq/sculpture/settings.hh>

namespace pixpq::sculpture {
  class listener {
  public:
    virtual void update(const std::string& name,const pixpq::sculpture::settings& settings) = 0;
  };

  class manager;
  class notifier : public pqxx::notification_receiver {
  public:
    notifier(manager* mgr, std::shared_ptr<listener> l);
    ~notifier();

    void operator() (const std::string& payload, int backend_pid);

  private:
    bool active;

    void listen_method();
    manager* mgr;
    std::shared_ptr<listener> l;
    std::thread listening_thread;
  };

  class manager {
  public:
    manager( const std::string& opts);

    void ensure_schema();

    void set_listener(std::shared_ptr<pixpq::sculpture::listener> l);

    void store(const std::string& name, const pixpq::sculpture::settings& loc);
    pixpq::sculpture::settings get(const std::string& name);

    pqxx::connection& get_notifier_connection();
  private:
    pqxx::connection connection;
    std::shared_ptr<notifier> n;
    pqxx::connection notifier_connection;
  };
}