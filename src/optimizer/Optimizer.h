#pragma once

#include "../utils/Config.h"
#include "../problem/Problem.h"

namespace optimizer {
  class Optimizer {
    public:
      Optimizer(utils::Config * config, problem::Problem * problem);
      virtual void run() = 0;

    protected:
      utils::Config * config;
      problem::Problem * problem;
  };
}
