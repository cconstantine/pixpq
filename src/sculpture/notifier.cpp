#include <pixpq/sculpture/manager.hh>


namespace pixpq::sculpture {
  template<typename T>
  notifier<T>::notifier(const std::string& channel, manager* mgr, std::shared_ptr<listener<T>> l) :
   pqxx::notification_receiver(mgr->get_notifier_connection(), channel),
   active(true), mgr(mgr), l(l), listening_thread(&notifier::listen_method, this) {  }

  template<typename T>
  notifier<T>::~notifier() {
    active = false;
    listening_thread.join();
  }

  template<typename T>
  void notifier<T>::operator() (const std::string& payload, int backend_pid) {
    l->update(payload, mgr->get<T>(payload));
  }

  template<typename T>
  void notifier<T>::listen_method() {
    while (active) {
      try {
        mgr->get_notifier_connection().await_notification();
      } catch (const pqxx::broken_connection& e) { /* ignore broken connections */ }
    }
  }
}
