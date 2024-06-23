#pragma once

#include <random>
#include <list>

#include "../utils/Config.h"
#include "../models/Place.h"
#include "Problem.h"
#include "SolutionFitness.h"

namespace problem {
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
      // std::vector<models::Place *> tour;

      Solution(Problem* problem);
      static Solution * random(Problem* problem, std::default_random_engine* randomEngine);

      void loopTour(std::function<bool(int i, int t_before, int t_in, int t_out, models::Place * place, TourPlaceType tourPlaceType)> lambda);
      SolutionFitness computeFitness(config::Fitness * config);
      SolutionCompliant getCompliancy();

      std::string toString();

      int travelTime();
      int totalTime();
      int totalPrice();
      float reviews(config::Fitness * config);
      int numberOfPlaces();

      bool hasDuplicates();
      void removeDuplicates();

      void repair(SolutionCompliant * compliant, std::default_random_engine* randomEngine);
  };
}
