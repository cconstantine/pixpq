#include <string>
#include <thread>

#include "libpq-fe.h"

namespace pixpq {

  class location {
  public:
    location(float x, float y, float z);

    float x, y, z;
  };

  class db {
  public:
    db();

    ~db();
    void send(const std::string& name, const pixpq::location& loc);

    inline virtual void receive(const std::string& name, const pixpq::location& loc) {};

    void listen();

  private:

    PGconn * ensure_connection(PGconn *conn);

    std::string conninfo;
    PGconn     *connection;
    std::thread listen_thread;
    
  };

}


