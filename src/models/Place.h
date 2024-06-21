#pragma once

#include <string>
#include "../lib/json.hpp"
using json = nlohmann::json;

#include "Map.h"

namespace models {
  enum class PlaceType {
    standard = 0,
    attraction = 1,
    cafe = 2,
    restaurant = 3,
    bar = 4,
    path = 5,
  };

  class Place {
    public: 
      std::string id;
      std::string name;

      float note; // Reviews average note
      int reviews; // Number of reviews

      int priority; // Visit priority (higher = better)
      int preference; // Personal preference (higher = better)

      int timeToVisit; // Minutes
      int open; // Open hour
      int close; // Close hour

      int price; // Cents

      Location locationA;
      Location locationB;

      PlaceType type = PlaceType::standard;

      Place(std::string id, std::string name, PlaceType type, float note, int reviews, Location location, int timeToVisit, int price, int open = 0, int close = -1, int priority = 1, int preference = 1);
  
      Place(json input);
      json to_json();

      std::string typeToString();
  };

}

