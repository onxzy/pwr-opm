#pragma once

#include "../problem/Problem.h"
#include "../problem/Solution.h"

namespace utils {
  class Stats {
    public:
      static void printStats(std::vector<problem::SolutionFitness> fitnesses);
  };
}
