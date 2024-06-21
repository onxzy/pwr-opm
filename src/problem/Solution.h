#pragma once

#include <random>
#include <list>

#include "../utils/Config.h"
#include "../models/Place.h"
#include "Problem.h"

namespace problem {
  class SolutionCompliant {
    public:
      int placesOpen = 0;
      bool placesUnique = true;

      int totalDuration = 0;
      int maxPrice = 0;

      bool breakfast = true;
      int breakfastStart = 0;
      int breakfastEnd = 0;

      bool lunch = true;
      int lunchStart = 0;
      int lunchEnd = 0;

      bool dinner = true;
      int dinnerStart = 0;
      int dinnerEnd = 0;

      bool bar = true;
      int barStart = 0;

      bool summary();

      std::string toString();
  };


  class SolutionFitness {
    public:
      SolutionCompliant compliant;

      float global = 0;
      float penalty = 0;
      float fitness = 0;

      bool operator<(const SolutionFitness& other) const;
      bool operator>(const SolutionFitness& other) const;
  };

  enum TourPlaceType {
    ATTRACTION,
    BREAKFAST,
    LUNCH,
    DINNER,
    BAR
  };

  class Solution {
    private:
      Problem* problem = nullptr;

      models::Place* pickPlace(int * t, std::vector<models::Place *>* places, int overrideDuration = 0);
      int currentTravelTimeToPlace(models::Place * nextPlace);

    public:
      std::list<models::Place*> tour;

      Solution(Problem* problem);
      static Solution * random(Problem* problem, std::default_random_engine* randomEngine);

      void loopTour(std::function<bool(int i, int t_before, int t_in, int t_out, models::Place * place, TourPlaceType tourPlaceType)> lambda);
      SolutionCompliant compliant();
      SolutionFitness fitness(config::Fitness * config);

      std::string toString();

      int travelTime();
      int totalTime();
      int totalPrice();
      float reviews(config::Fitness * config);
      int numberOfPlaces();

      bool hasDuplicates();
      void removeDuplicates();
  };
}
