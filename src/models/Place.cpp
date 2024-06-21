#include <iostream>
#include "Place.h"

namespace models {
  Place::Place(std::string id, std::string name, PlaceType type, float note, int reviews, Location location, int timeToVisit, int price, int open, int close, int priority, int preference)
    : locationA(location), locationB(location) {
    this->id = id;
    this->name = name;
    this->type = type;

    this->note = note;
    this->reviews = reviews;
    this->timeToVisit = timeToVisit;

    this->priority = priority;
    this->preference = preference;

    this->open = open;
    this->close = close;

    this->price = price;
  }

  Place::Place(json input)
  : locationA(input["locationA"]), locationB(input["locationB"]) {
    this->id = input["id"];
    this->name = input["name"];
    this->type = input["type"];

    this->note = input["note"];
    this->reviews = input["reviews"];
    this->timeToVisit = input["timeToVisit"];

    this->priority = input["priority"];
    this->preference = input["preference"];

    this->open = input["open"];
    this->close = input["close"];

    this->price = input["price"];
  }

  json Place::to_json() {
    return {
      {"id", this->id},
      {"name", this->name},
      {"type", this->type},
      {"note", this->note},
      {"reviews", this->reviews},
      {"priority", this->priority},
      {"preference", this->preference},
      {"timeToVisit", this->timeToVisit},
      {"open", this->open},
      {"close", this->close},
      {"price", this->price},
      {"locationA", this->locationA.to_json()},
      {"locationB", this->locationB.to_json()},
    };
  }

  std::string Place::typeToString() {
    switch (this->type) {
      case PlaceType::attraction:
        return "Attraction";
      case PlaceType::restaurant:
        return "Restaurant";
      case PlaceType::bar:
        return "Bar";
      case PlaceType::cafe:
        return "Cafe";
      default:
        return "Unknown";
    }
  }
};