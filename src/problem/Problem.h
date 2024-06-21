#pragma once

#include <list>
#include <set>
#include <random>
#include <boost/numeric/ublas/symmetric.hpp>
#include "../lib/json.hpp"
using json = nlohmann::json;

#include "../models/Place.h"
#include "../models/Map.h"

namespace problem {

  class Constraints {
    public:
      models::Location* start = nullptr;
      models::Location* end = nullptr;

      // Override on meals constraints
      int startTime = 7*60;
      int totalDuration = 900;
      int maxPrice = -1;

      bool breakfast = true;
      int breakfastStart = 8*60;
      int breakfastEnd = 10*60;
      int breakfastDuration = 60;
      
      bool lunch = true;
      int lunchStart = 12*60;
      int lunchEnd = 14*60;
      int lunchDuration = 60;

      bool dinner = true;
      int dinnerStart = 19*60;
      int dinnerEnd = 22*60;
      int dinnerDuration = 60;

      bool bar = true;
      int barStart = 21*60;
      int barDuration = 60;

      Constraints(models::Location* start = nullptr, models::Location* end = nullptr);

      Constraints(json input);
      json to_json();
  };

  class Problem {
    public:
      Constraints* constraints;

      std::vector<models::Place*> places;
      boost::numeric::ublas::symmetric_matrix<int> placesTravelTimes;

      Problem(Constraints* constraints, std::vector<models::Place*> places, boost::numeric::ublas::symmetric_matrix<int> placesTravelTimes);
  
      Problem(json input);
      json to_json();

      void dump(std::string filename);
      static Problem load(std::string filename);

      static Problem generate(int nbPlaces, int citySize, int seed = 1);
      static void generateBulk(std::string outputFolder, int length, int nbPlaces, int citySize, int startSeed = 1);
      static std::vector<Problem> loadBulk(std::string folder, int nbToLoad = -1);

      std::vector<models::Place *> findPlacesByType(models::PlaceType type, std::set<models::Place*> exclude = std::set<models::Place*>());
      int getTravelTime(models::Location * locationA, models::Location * locationB);
  };
};
