#include <pixpq/pixpq.hpp>

class pixo_connect : public pixpq::db {
  public:
    void send(const std::string& name, const pixpq::location& loc) {
      start_time = std::chrono::high_resolution_clock::now();
      pixpq::db::send(name, loc);
    }
    virtual void receive(const std::string& name, const pixpq::location& loc) {
      std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
      std::chrono::duration<float> time_duration = now - start_time;
      printf("%3.2fms: %s at %f, %f, %f\n", time_duration.count() * 1000, name.c_str(), loc.x, loc.y, loc.z);
    }

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

int
main(int argc, char **argv)
{
  pixo_connect db;
  int i = 0;
  while(true) {
    db.send("foo", pixpq::location(++i, 2, 3));
    std::this_thread::sleep_for(std::chrono::duration<float>(1.0 / 30.0));
  }

  fprintf(stderr, "Done.\n");

  return 0;
}