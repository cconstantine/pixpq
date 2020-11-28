#include <pixpq/pixpq.hpp>

class test_tracking_listener : public pixpq::listener<pixpq::tracking::location> {
public:
  virtual void update(const std::string& name, const pixpq::tracking::location& loc) {
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_duration = now - start_time;

    printf("%3.2fms: %s at %f, %f, %f\n", time_duration.count() * 1000,   name.c_str(), loc.x, loc.y, loc.z);
  }
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

class test_settings_listener 
: public pixpq::listener<pixpq::sculpture::settings>,
  public pixpq::listener<pixpq::sculpture::pattern> {
public:
  virtual void update(const std::string& name, const pixpq::sculpture::settings& s) {
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_duration = now - start_time;

    printf("%3.2fms: %s (%s) %f, %f\n", time_duration.count() * 1000,   name.c_str(), s.active_pattern.c_str(), s.brightness, s.gamma);
  }
  virtual void update(const std::string& name, const pixpq::sculpture::pattern& s) { }

  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

int
main(int argc, char **argv)
{
  fprintf(stderr, "make manager\n");
  pixpq::manager manager("");
  fprintf(stderr, "ensure schema\n");
  manager.ensure_schema();

  fprintf(stderr, "make tracking listener\n");
  std::shared_ptr<test_tracking_listener> ttl = std::make_shared<test_tracking_listener>();
  fprintf(stderr, "set tracking listener\n");
  manager.set_listener<pixpq::tracking::location>(ttl);

  fprintf(stderr, "make settings_listener listener\n");
  std::shared_ptr<test_settings_listener> tsl = std::make_shared<test_settings_listener>();
  fprintf(stderr, "set settings_listener listener\n");
  manager.set_listener<pixpq::sculpture::settings>(tsl);
  manager.start_listening();
  fprintf(stderr, "start loop\n");
  {
    int i = 0;
    while(i < 10) {

      fprintf(stderr, "store ttl\n");
      ttl->start_time = std::chrono::high_resolution_clock::now();
      manager.store<pixpq::tracking::location>("foo", pixpq::tracking::location(++i, 2, 3));

      tsl->start_time = std::chrono::high_resolution_clock::now();
      manager.store<pixpq::sculpture::settings>("foo", pixpq::sculpture::settings("asldkfj", 2.0 + i, 3.0 + i));

      std::this_thread::sleep_for(std::chrono::duration<float>(1.0 / 30.0));
    }  
  }
  // {

  //   try {
  //     pixpq::sculpture::settings settings = manager.get<pixpq::sculpture::settings>("foo");
  //   } catch( const pqxx::unexpected_rows& e) {}
    
  //   int i = 0;
  //   while(++i < 10) {
  //     tsl->start_time = std::chrono::high_resolution_clock::now();
  //     manager.store<pixpq::sculpture::settings>("foo", pixpq::sculpture::settings("asldkfj", 2.0 + i, 3.0 + i));

  //     std::this_thread::sleep_for(std::chrono::duration<float>(1.0 / 60.0));
  //   }  
  // }

  fprintf(stderr, "Done.\n");

  return 0;
}