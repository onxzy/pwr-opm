#pragma once

#include "../utils/Config.h"
#include "../utils/Stats.h"
#include "../problem/Problem.h"

namespace optimizer {
  class Optimizer {
    public:
      Optimizer(utils::Config * config, problem::Problem * problem);
      virtual problem::Solution * run() = 0;
      utils::Stats * getStats();

    protected:
      utils::Config * config;
      utils::Stats stats;

      problem::Problem * problem;
  };
}
