#include "Optimizer.h"

namespace optimizer {
  Optimizer::Optimizer(utils::Config * config, problem::Problem * problem) {
    this->config = config;
    this->problem = problem;
  };
}
