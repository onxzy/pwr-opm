#pragma once

#include <random>

#include "Optimizer.h"
#include "FitnessMap.h"

#include "../problem/Solution.h"

namespace optimizer::random {
  class Random: public Optimizer {
    public:
      Random(utils::Config * config, problem::Problem * problem, std::default_random_engine * e);
      problem::Solution * run();

    private:
      std::default_random_engine * e;

      std::vector<problem::Solution *> population;

      FitnessMap * fitnessMap;
  };
}

