#pragma once
#include <string>

namespace pixpq::sculpture {
  class pattern {
  public:
    pattern(const std::string& code, bool enabled);

    std::string code;
    bool enabled;
  };
}