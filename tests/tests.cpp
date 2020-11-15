#include <pixpq/pixpq.hh>

class test_tracking_listener : public pixpq::tracking::listener {
public:
  virtual void update(const std::string& name, const pixpq::tracking::location& loc) {
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_duration = now - start_time;

    printf("%3.2fms: %s at %f, %f, %f\n", time_duration.count() * 1000,   name.c_str(), loc.x, loc.y, loc.z);
  }
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

class test_settings_listener : public pixpq::sculpture::listener {
public:
  virtual void update(const std::string& name, const pixpq::sculpture::settings& s) {
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_duration = now - start_time;

    printf("%3.2fms: %s (%s) %f, %f\n", time_duration.count() * 1000,   name.c_str(), s.active_pattern.c_str(), s.brightness, s.gamma);
  }
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

int
main(int argc, char **argv)
{
  {
    std::shared_ptr<test_tracking_listener> listener = std::make_shared<test_tracking_listener>();
    pixpq::tracking::manager location_manager("");

    location_manager.ensure_schema();
    location_manager.set_listener(listener);
    int i = 0;
    while(i < 10) {
      listener->start_time = std::chrono::high_resolution_clock::now();
      location_manager.store("foo", pixpq::tracking::location(++i, 2, 3));

      std::this_thread::sleep_for(std::chrono::duration<float>(1.0 / 30.0));
    }  
  }
  {
    std::shared_ptr<test_settings_listener> listener = std::make_shared<test_settings_listener>();
    pixpq::sculpture::manager sculpture_manager("");

    sculpture_manager.ensure_schema();
    sculpture_manager.set_listener(listener);
    int i = 0;
    while(++i < 10) {
      listener->start_time = std::chrono::high_resolution_clock::now();
      sculpture_manager.store("foo", pixpq::sculpture::settings("asldkfj", 2.0 + i, 3.0 + i));

      std::this_thread::sleep_for(std::chrono::duration<float>(1.0 / 60.0));
    }  
  }

  fprintf(stderr, "Done.\n");

  return 0;
}