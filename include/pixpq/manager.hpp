#pragma once
#include <pqxx/pqxx>
#include <thread>
#include <mutex>
#include <condition_variable>
namespace pixpq {
  template<typename T>
  class listener {
  public:
    virtual void update(const std::string& name,const T& t) = 0;
  };

  class manager {
  public:
    manager( const std::string& opts);
    ~manager();

    void ensure_schema();

    template<typename T>
    void set_listener(std::shared_ptr<listener<T>> l);

    template<typename T>
    void store(const std::string& name, const T& loc);
    
    template<typename T>
    T get(const std::string& name);

    template<typename T>
    std::map<std::string, T> get_all();

    pqxx::connection& get_notifier_connection();

    void process_updates_background();
    void process_updates();

  private:
    pqxx::connection connection;
    pqxx::connection notifier_connection;

    std::mutex connection_mutex;
    std::map<std::string, std::shared_ptr<pqxx::notification_receiver>> notifiers;

    std::condition_variable listen_latch;
    std::mutex listen_mutex;
    bool is_listening;

    bool active;
    void listen_method();
    std::thread listening_thread;
  };

}
