#include <random>

#include "../../lib/print.h"

#include "../GA.h"

namespace optimizer::ga::mutation {
  Mutation::Mutation(utils::Config *config, std::default_random_engine * e) {
    this->config = config;
    this->e = e;
  }

  SwapMutation::SwapMutation(utils::Config *config, std::default_random_engine * e) : Mutation(config, e) {}

  problem::Solution *SwapMutation::run(problem::Solution *solution) {
    PRINTLN("swap mutation");
    return nullptr;
  }
}