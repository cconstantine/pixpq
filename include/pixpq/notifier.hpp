#pragma once
#include <pixpq/manager.hpp>

namespace pixpq {
  template<typename T>
  class notifier : public pqxx::notification_receiver {
  public:
    notifier(const std::string& channel, manager* mgr, std::shared_ptr<listener<T>> l);
    ~notifier();

    void operator() (const std::string& payload, int backend_pid);

  private:
    manager* mgr;
    std::shared_ptr<listener<T>> l;
  };

  template<typename T>
  notifier<T>::notifier(const std::string& channel, manager* mgr, std::shared_ptr<listener<T>> l) :
   pqxx::notification_receiver(mgr->get_notifier_connection(), channel),
   mgr(mgr), l(l) {  }

  template<typename T>
  notifier<T>::~notifier() {
  }

  template<typename T>
  void notifier<T>::operator() (const std::string& payload, int backend_pid) {
    l->update(payload, mgr->get<T>(payload));
  }

}
