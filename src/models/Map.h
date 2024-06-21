#pragma once

#include "../lib/json.hpp"
using json = nlohmann::json;

namespace models {
  class Location {
    public:
      int index;

      int x;
      int y;

      Location(int index, int x, int y);

      Location(json input);
      json to_json();
  };
}

