#include <iostream>
#include <fstream>
#include <iterator>
#include <cmath>
#include <filesystem>

#include <boost/numeric/ublas/symmetric.hpp>
using namespace boost::numeric::ublas;

#include "Problem.h"

namespace problem {
  Constraints::Constraints(models::Location* start, models::Location* end) {
    this->start = start;
    this->end = end;
  }

  Constraints::Constraints(json input) {
    if (input.contains("start") && !input["start"].is_null())
      this->start = new models::Location(input["start"]);
    if (input.contains("end") && !input["end"].is_null())
      this->end = new models::Location(input["end"]);

    this->startTime = input["startTime"];
    this->totalDuration = input["totalDuration"];
    this->maxPrice = input["maxPrice"];

    this->breakfast = input["breakfast"];
    this->breakfastStart = input["breakfastStart"];
    this->breakfastEnd = input["breakfastEnd"];
    this->breakfastDuration = input["breakfastDuration"];

    this->lunch = input["lunch"];
    this->lunchStart = input["lunchStart"];
    this->lunchEnd = input["lunchEnd"];
    this->lunchDuration = input["lunchDuration"];

    this->dinner = input["dinner"];
    this->dinnerStart = input["dinnerStart"];
    this->dinnerEnd = input["dinnerEnd"];
    this->dinnerDuration = input["dinnerDuration"];

    this->bar = input["bar"];
    this->barStart = input["barStart"];
    this->barDuration = input["barDuration"];
  }

  json Constraints::to_json() {
    return {
      {"start", this->start != nullptr ? this->start->to_json() : nullptr},
      {"end", this->end != nullptr ? this->end->to_json() : nullptr},

      {"startTime", this->startTime},
      {"totalDuration", this->totalDuration},
      // {"maxPrice", this->maxPrice},
      {"maxPrice", 0},

      {"breakfast", this->breakfast},
      {"breakfastStart", this->breakfastStart},
      {"breakfastEnd", this->breakfastEnd},
      {"breakfastDuration", this->breakfastDuration},

      {"lunch", this->lunch},
      {"lunchStart", this->lunchStart},
      {"lunchEnd", this->lunchEnd},
      {"lunchDuration", this->lunchDuration},

      {"dinner", this->dinner},
      {"dinnerStart", this->dinnerStart},
      {"dinnerEnd", this->dinnerEnd},
      {"dinnerDuration", this->dinnerDuration},

      {"bar", this->bar},
      {"barStart", this->barStart},
      {"barDuration", this->barDuration}
    };
  }

  Problem::Problem(Constraints* constraints, std::vector<models::Place*> places, boost::numeric::ublas::symmetric_matrix<int> placesTravelTimes) {
    this->constraints = constraints;
    this->places = places;
    this->placesTravelTimes = placesTravelTimes;
  }

  Problem::Problem(json input) {
    this->constraints = new Constraints(input["constraints"]);

    this->places = std::vector<models::Place*>();
    if (input["places"].is_array()) {
      for (auto &place_json : input["places"]) {
        this->places.push_back(new models::Place(place_json));
      }
    }

    this->placesTravelTimes = boost::numeric::ublas::symmetric_matrix<int>(input["placesTravelTimes"].size());
    for (int i = 0; i < (int) this->placesTravelTimes.size1(); ++ i)
      for (int j = 0; j <= i; ++ j)
        this->placesTravelTimes(i, j) = 0;
    
    if (input["placesTravelTimes"].is_array()) {
      int i = 0;
      for (auto &line : input["placesTravelTimes"].items()) {
        int j = 0;
        for (auto &element : line.value().items()) {
          this->placesTravelTimes(i, j) = element.value();
          ++ j;
        }
        ++ i;
      }
    }
  }

  json Problem::to_json() {
    json problem_json = {
      {"constraints", this->constraints->to_json()},
      {"places", json::array()},
      {"placesTravelTimes", json::array()},
    };

    for (auto &place : this->places) {
      problem_json["places"].push_back(place->to_json());
    }

    for (int i = 0; i < (int) this->placesTravelTimes.size1(); ++i) {
      json line = json::array();
      for (int j = 0; j < (int) this->placesTravelTimes.size2(); ++j) {
        line.push_back(this->placesTravelTimes(i, j));
      }
      problem_json["placesTravelTimes"].push_back(line);
    }

    return problem_json;
  }

  void Problem::dump(std::string filename) {
    std::ofstream outputFile;
    outputFile.open(filename);
    outputFile << this->to_json().dump(2);
    outputFile.close();
  }

  Problem Problem::load(std::string filename) {
    std::ifstream inputFile(filename);
    return Problem(json::parse(inputFile));
  }

  models::PlaceType randomPlaceType(std::default_random_engine e) {
    std::unordered_map<models::PlaceType, float> probabilities = {
      {models::PlaceType::standard, 0.0f},
      {models::PlaceType::path, 0.0f},
      {models::PlaceType::attraction, 0.4f},
      {models::PlaceType::restaurant, 0.2f},
      {models::PlaceType::cafe, 0.2f},
      {models::PlaceType::bar, 0.2f}
    };

    std::uniform_real_distribution<float> distrib(0, 1);

    float randomFloat = distrib(e);
    float cumulativeProbability = 0;

    for (const auto &entry : probabilities) {
        cumulativeProbability += entry.second;
        if (randomFloat < cumulativeProbability) {
            return entry.first;
        }
    }

    return static_cast<models::PlaceType>(0);  // This line should never be reached
}

  Problem Problem::generate(int nbPlaces, int citySize, int seed) {
    std::default_random_engine e(seed);

    std::uniform_int_distribution<int> flip_coin(0, 1);

    std::uniform_int_distribution<int> location_distrib(0, citySize);
    std::uniform_int_distribution<int> time_distrib(0, (24*60)-1);

    std::uniform_int_distribution<int> morning_opening_distrib(8*60, 10*60);
    std::uniform_int_distribution<int> afternoon_opening_distrib(12*60, 16*60);
    std::uniform_int_distribution<int> afternoon_closing_distrib(16*60, 19*60);
    std::uniform_int_distribution<int> evening_closing_distrib(22*60, 26*60);
    std::uniform_int_distribution<int> restaurant_opening_distrib(11*60, 17*60);
    std::uniform_int_distribution<int> restaurant_closing_distrib(18*60, 21*60);

    std::uniform_int_distribution<int> breakfast_distrib(7*60, 10*60);
    std::uniform_int_distribution<int> lunch_distrib(12*60, 14*60);
    std::uniform_int_distribution<int> dinner_distrib(17*60, 20*60);
    std::uniform_int_distribution<int> bar_distrib(20*60, 24*60-1);
  
    std::uniform_int_distribution<int> start_distrib(7*60, 10*60);
    std::uniform_int_distribution<int> duration_distrib(30, 4*60);
    std::uniform_int_distribution<int> meal_duration_distrib(60, 3*60);

    std::uniform_int_distribution<int> total_price_distrib(10*100, 200*100);

    std::uniform_real_distribution<float> note_distrib(1, 5);
    std::uniform_int_distribution<int> reviews_distrib(10, 5000);
    std::uniform_int_distribution<int> price_distrib(100, 50*100);

    int locationIndex = 0;
    int placeIndex = 0;

    // Constraints

    std::vector<models::Location*> locations;

    models::Location* start = nullptr;
    models::Location* end = nullptr;

    if (flip_coin(e)) {
      start = new models::Location(locationIndex++, location_distrib(e), location_distrib(e));
      locations.push_back(start);
      if (flip_coin(e)) {
        end = new models::Location(locationIndex++, location_distrib(e), location_distrib(e));
        locations.push_back(end);
      }
    }

    Constraints * constraints = new Constraints(start, end);

    constraints->startTime = start_distrib(e);

    if (flip_coin(e)) {
      constraints->maxPrice = total_price_distrib(e);
    } else { constraints->maxPrice = -1; }
  
    if (flip_coin(e)) {
      constraints->breakfast = true;
      constraints->breakfastStart = breakfast_distrib(e);
      constraints->breakfastEnd = constraints->breakfastStart + 2 * 60;
      constraints->breakfastDuration = meal_duration_distrib(e);
    } else { constraints->breakfast = false; }

    if (flip_coin(e)) {
      constraints->lunch = true;
      constraints->lunchStart = lunch_distrib(e);
      constraints->lunchEnd = constraints->lunchStart + 2 * 60;
      constraints->lunchDuration = meal_duration_distrib(e);
    } else { constraints->lunch = false; }

    if (flip_coin(e)) {
      constraints->dinner = true;
      constraints->dinnerStart = dinner_distrib(e);
      constraints->dinnerEnd = constraints->dinnerStart + 2 * 60;
      constraints->dinnerDuration = meal_duration_distrib(e);
    } else { constraints->dinner = false; }

    if (flip_coin(e)) {
      constraints->bar = true;
      constraints->barStart = bar_distrib(e);
      constraints->barDuration = duration_distrib(e);
    } else { constraints->bar = false; }

    std::uniform_int_distribution<int> total_duration_distrib(
      (
        constraints->breakfast ? constraints->breakfastEnd
        : constraints->lunch ? constraints->lunchEnd
        : constraints->dinner ? constraints->dinnerEnd
        : constraints->bar ? constraints->barStart
        : constraints->startTime + 4*60
      ) - constraints->startTime, 20*60);
    constraints->totalDuration = total_duration_distrib(e);

    // Problem

    std::vector<models::Place*> places;
    for (int i = 0; i < nbPlaces; ++i) {
      models::Location* location = new models::Location(locationIndex++, location_distrib(e), location_distrib(e));
      locations.push_back(location);
      
      models::PlaceType placeType = randomPlaceType(e);

      places.push_back(
        new models::Place(
          std::to_string(placeIndex++),
          std::to_string(placeIndex),
          placeType,
          note_distrib(e),
          reviews_distrib(e),
          *location,
          duration_distrib(e),
          price_distrib(e),
          placeType == models::PlaceType::restaurant ? restaurant_opening_distrib(e) : placeType == models::PlaceType::bar ? afternoon_opening_distrib(e) : morning_opening_distrib(e),
          placeType == models::PlaceType::restaurant ? restaurant_closing_distrib(e) : placeType == models::PlaceType::bar ? evening_closing_distrib(e) : afternoon_closing_distrib(e)
          ));
    }

    symmetric_matrix<int> placesTravelTimes(locationIndex, locationIndex);
    for (int i = 0; i < (int) placesTravelTimes.size1(); ++ i)
      for (int j = 0; j <= i; ++ j)
        placesTravelTimes(i, j) = 0;

    const int travelingSpeed = 83 ; // arround 5 (km/h) * 1000 / 60;

    for (int i = 0; i < locationIndex; ++ i) {
      for (int j = 0; j < i; ++ j) {
        models::Location* locationA = locations[i];
        models::Location* locationB = locations[j];

        placesTravelTimes(i, j) = static_cast<int>(sqrt(pow(locationA->x - locationB->x, 2) + pow(locationA->y - locationB->y, 2)) / travelingSpeed);
      }
    }

    return Problem(constraints, places, placesTravelTimes);
  }

  void Problem::generateBulk(std::string outputFolder, int length, int nbPlaces, int citySize, int startSeed) {
    const std::string folder = outputFolder + "/"
        + std::to_string(nbPlaces) + "/"
        + std::to_string(citySize) + "/";
    
    if (!std::filesystem::is_directory(folder) || !std::filesystem::exists(folder)) {
      std::filesystem::create_directories(folder);
    }
    
    for (int i = 0; i < length; ++ i) {
      Problem::generate(nbPlaces, citySize, startSeed + i).dump(folder + std::to_string(i) + ".json");
    }
  }

  std::vector<Problem> Problem::loadBulk(std::string folder, int nbToLoad) {
    std::vector<Problem> problems;

    int i = 0;
    for (const auto & entry : std::filesystem::directory_iterator(folder)) {
      problems.push_back(Problem::load(entry.path().string()));
      i++;
      if (nbToLoad > 0 && i >= nbToLoad) break;
    }

    return problems;
  }

  std::vector<models::Place*> Problem::findPlacesByType(models::PlaceType type, std::set<models::Place*> exclude) {
    return Problem::findPlacesByType(&this->places, type, exclude);
  }

  std::vector<models::Place*> Problem::findPlacesByType(std::vector<models::Place *> * places, models::PlaceType type, std::set<models::Place*> exclude) {
    std::vector<models::Place*> output;

    for (auto it = places->begin(); it != places->end(); ++it) {
      if (exclude.contains(*it)) continue;
      if ((*it)->type == type) output.push_back(*it);
    }

    return output;
  }

  int Problem::getTravelTime(models::Location * locationA, models::Location * locationB) {
    return this->placesTravelTimes(locationA->index, locationB->index);
  }
}