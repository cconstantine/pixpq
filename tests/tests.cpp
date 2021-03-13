#include <pixpq/pixpq.hpp>

class test_settings_listener 
: public pixpq::listener<pixpq::sculpture::settings>,
  public pixpq::listener<pixpq::sculpture::pattern>,
  public pixpq::listener<pixpq::tracking::location> {
public:
  virtual void update(const std::string& name, const pixpq::sculpture::settings& s) {
    printf("update settings: %s (%s) %f, %f\n", name.c_str(), s.active_pattern.c_str(), s.brightness, s.gamma);
  }
  virtual void update(const std::string& name, const pixpq::sculpture::pattern& p) {
    printf("update pattern: %s (%d):\n'%s'\n", name.c_str(), p.enabled, p.glsl_code.c_str());
  }
  virtual void update(const std::string& name, const pixpq::tracking::location& loc) {
    printf("update location: %s at %f, %f, %f\n", name.c_str(), loc.x, loc.y, loc.z);
  }
};

int
main(int argc, char **argv)
{
  pixpq::manager manager("");
  manager.ensure_schema();

  std::shared_ptr<test_settings_listener> tsl = std::make_shared<test_settings_listener>();
  manager.set_listener<pixpq::tracking::location>(tsl);
  manager.set_listener<pixpq::sculpture::settings>(tsl);
  manager.set_listener<pixpq::sculpture::pattern>(tsl);

  pixpq::tracking::location l = manager.get<pixpq::tracking::location>(pixpq::tracking::location::by_name("foo"));
  printf("%s: %f, %f, %f\n", l.name.c_str(), l.x, l.y, l.z);
  l.x++;
  l = manager.get<pixpq::tracking::location>(pixpq::tracking::location::upsert(l));
  printf("%s: %f, %f, %f\n", l.name.c_str(), l.x, l.y, l.z);

  manager.process_updates();

  pixpq::sculpture::settings settings = manager.get<pixpq::sculpture::settings>(pixpq::sculpture::settings::get_or_create("foo", "", 1, 1));
  pixpq::sculpture::fadecandy fc = manager.get<pixpq::sculpture::fadecandy>(pixpq::sculpture::fadecandy::get_or_create(settings, "localhost"));
  std::list<pixpq::sculpture::led> leds;
  for(int i = 0;i < 10;i++) {
    leds.push_back(pixpq::sculpture::led(0, i, 0));
  }

  manager.get_all<pixpq::sculpture::led>(pixpq::sculpture::led::upsert(fc, leds));

  {
    int i = 0;
    while(i < 3) {      
      pixpq::tracking::location l = manager.get<pixpq::tracking::location>(pixpq::tracking::location::by_name("foo"));
      l.x = ++i;
      manager.get<pixpq::tracking::location>(pixpq::tracking::location::upsert(l));
      // manager.get<pixpq::sculpture::settings>("foo", pixpq::sculpture::settings("asldkfj", 2.0 + i, 3.0 + i));

      pixpq::sculpture::pattern p("foo.glsl", std::string("aldksfj: " + std::to_string(i)), i % 2 == 0);
      manager.get<pixpq::sculpture::pattern>(pixpq::sculpture::pattern::upsert(p));

      std::this_thread::sleep_for(std::chrono::duration<float>(1.0 / 60.0));
      manager.process_updates();
    }  
  }

  // for (auto iter : manager.get_all<std::string, pixpq::tracking::location>()) {
  //   printf("location: %s: %f, %f, %f\n", iter.first.c_str(), iter.second.x, iter.second.y, iter.second.z);
  // }

  // for (auto iter : manager.get_all<std::string, pixpq::sculpture::settings>()) {
  //   printf("settings: %s: %s, %f, %f\n", iter.first.c_str(), iter.second.active_pattern.c_str(), iter.second.brightness, iter.second.gamma);
  // }

  // for (auto iter : manager.get_all<std::string, pixpq::sculpture::pattern>()) {
  //   printf("pattern:\n%s: %s, %d\n", iter.first.c_str(), iter.second.glsl_code.c_str(), iter.second.enabled);
  // }

  // pixpq::sculpture::settings s("cool_pattern.glsl", 1, 1);
  // manager.store<void, pixpq::sculpture::settings>("thingy", s);

  // pixpq::sculpture::fadecandy fc("place.local");

  // // std::map<std::string, std::vector<pixpq::sculpture::fadecandy>> fcs = manager.get_all<std::string, pixpq::sculpture::fadecandy>();

  // printf("fc.id: %d\n", fc.id);
  // fc.id = manager.store<int, pixpq::sculpture::fadecandy>("thingy2", fc);
  // printf("fc.id: %d\n", fc.id);



  printf("Done.\n");

  return 0;
}