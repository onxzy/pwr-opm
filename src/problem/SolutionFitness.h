#pragma once

#include <string>

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
}
