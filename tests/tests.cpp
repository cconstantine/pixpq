#include <pixpq/pixpq.hpp>

class test_tracking_listener : public pixpq::listener<pixpq::tracking::location> {
public:
  virtual void update(const std::string& name, const pixpq::tracking::location& loc) {
    printf("%s at %f, %f, %f\n", name.c_str(), loc.x, loc.y, loc.z);
  }
};

class test_settings_listener 
: public pixpq::listener<pixpq::sculpture::settings>,
  public pixpq::listener<pixpq::sculpture::pattern> {
public:
  virtual void update(const std::string& name, const pixpq::sculpture::settings& s) {
    printf("%s (%s) %f, %f\n", name.c_str(), s.active_pattern.c_str(), s.brightness, s.gamma);
  }
  virtual void update(const std::string& name, const pixpq::sculpture::pattern& p) {
    printf("%s (%d):\n'%s'\n", name.c_str(), p.enabled, p.glsl_code.c_str());
  }
};

int
main(int argc, char **argv)
{
  pixpq::manager manager("");
  manager.ensure_schema();

  std::shared_ptr<test_tracking_listener> ttl = std::make_shared<test_tracking_listener>();
  manager.set_listener<pixpq::tracking::location>(ttl);

  std::shared_ptr<test_settings_listener> tsl = std::make_shared<test_settings_listener>();
  manager.set_listener<pixpq::sculpture::settings>(tsl);
  manager.set_listener<pixpq::sculpture::pattern>(tsl);

  fprintf(stderr, "start loop: %p\n", std::this_thread::get_id());
  {
    int i = 0;
    while(i < 10) {
      fprintf(stderr, "store\n");
      manager.store<pixpq::tracking::location>("foo", pixpq::tracking::location(++i, 2, 3));
      manager.store<pixpq::sculpture::settings>("foo", pixpq::sculpture::settings("asldkfj", 2.0 + i, 3.0 + i));
      manager.store<pixpq::sculpture::pattern>("foo.glsl", pixpq::sculpture::pattern(std::string("aldksfj: " + std::to_string(i)), i % 2 == 0));

      std::this_thread::sleep_for(std::chrono::duration<float>(1.0 / 60.0));
      manager.process_updates();
    }  
  }

  for (auto iter : manager.get_all<pixpq::tracking::location>()) {
    printf("location: %s: %f, %f, %f\n", iter.first.c_str(), iter.second.x, iter.second.y, iter.second.z);
  }

  for (auto iter : manager.get_all<pixpq::sculpture::settings>()) {
    printf("settings: %s: %s, %f, %f\n", iter.first.c_str(), iter.second.active_pattern.c_str(), iter.second.brightness, iter.second.gamma);
  }

  for (auto iter : manager.get_all<pixpq::sculpture::pattern>()) {
    printf("pattern:\n%s: %s, %d\n", iter.first.c_str(), iter.second.glsl_code.c_str(), iter.second.enabled);
  }

  fprintf(stderr, "Done.\n");

  return 0;
}