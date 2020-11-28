#pragma once
#include <string>

namespace pixpq::sculpture {
  class pattern {
  public:
    pattern(const std::string& glsl_code, bool enabled);

    std::string glsl_code;
    bool enabled;
  };
}