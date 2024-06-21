#include "Map.h"
#include <iostream>

namespace models {
  Location::Location(int index, int x, int y) {
    this->index = index;
    this->x = x;
    this->y = y;
  }

  Location::Location(json input) {
    this->index = input["index"];
    this->x = input["x"];
    this->y = input["y"];
  }

  json Location::to_json() {
    return {
      {"index", this->index},
      {"x", this->x},
      {"y", this->y}
    };
  }
}
