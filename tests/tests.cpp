#include <pixpq/pixpq.hh>

class test_listener : public pixpq::tracking::listener {
public:
  virtual void update(const std::string& name, const pixpq::tracking::location& loc) {
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_duration = now - start_time;

    printf("%3.2fms: %s at %f, %f, %f\n", time_duration.count() * 1000,   name.c_str(), loc.x, loc.y, loc.z);
  }
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

int
main(int argc, char **argv)
{
  std::shared_ptr<test_listener> listener = std::make_shared<test_listener>();
  pixpq::tracking::manager location_manager("");

  location_manager.ensure_schema();
  location_manager.set_listener(listener);
  int i = 0;
  while(i < 100) {
    listener->start_time = std::chrono::high_resolution_clock::now();
    location_manager.store("foo", pixpq::tracking::location(++i, 2, 3));

    std::this_thread::sleep_for(std::chrono::duration<float>(1.0 / 30.0));
  }

  fprintf(stderr, "Done.\n");

  return 0;
}