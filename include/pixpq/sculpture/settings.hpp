#pragma once
#include <string>

namespace pixpq::sculpture {
  class settings {
  public:
    settings(const std::string& active_pattern, float brightness, float gamma);

    std::string active_pattern;
    float brightness, gamma;
  };
}