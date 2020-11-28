#pragma once

namespace pixpq::tracking {
  class location {
  public:
    location(float x, float y, float z);

    float x, y, z;
  };
}