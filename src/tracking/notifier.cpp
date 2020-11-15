#include <pixpq/tracking/manager.hh>


namespace pixpq::tracking {

  notifier::notifier(manager* mgr, std::shared_ptr<listener> l) :
   pqxx::notification_receiver(mgr->get_notifier_connection(), "tracking_location_update"),
   active(true), mgr(mgr), l(l), listening_thread(&notifier::listen_method, this) {  }

  notifier::~notifier() {
    active = false;
    listening_thread.join();
  }
  void notifier::operator() (const std::string& payload, int backend_pid) {
    l->update(payload, mgr->get(payload));
  }

  void notifier::listen_method() {
    while (active) {
      try {
        mgr->get_notifier_connection().await_notification();
      } catch (const pqxx::broken_connection& e) { /* ignore broken connections */ }
    }
  }
}
