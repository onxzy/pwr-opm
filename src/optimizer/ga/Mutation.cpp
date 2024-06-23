#include <random>

#include "../../lib/print.h"

#include "../GA.h"

namespace optimizer::ga::mutation {
  Mutation::Mutation(utils::Config *config, std::default_random_engine * e) {
    this->config = config;
    this->e = e;
  }

  SwapMutation::SwapMutation(utils::Config *config, std::default_random_engine * e) : Mutation(config, e) {}

  void SwapMutation::run(problem::Solution *solution) {
    if (solution->tour.size() < 2) return;
    if (std::uniform_real_distribution<float>(0, 1)(*this->e) > this->config->optimizer.ga.mutation.probability) {
      return;
    }

    int i = std::uniform_int_distribution<int>(0, solution->tour.size() - 1)(*this->e);
    int j;
    do {
      j = std::uniform_int_distribution<int>(0, solution->tour.size() - 1)(*this->e);
    } while (i == j);

    // Swap two element in the list using splice
    solution->tour.splice(std::next(solution->tour.begin(), i), solution->tour, std::next(solution->tour.begin(), j));
  }
}