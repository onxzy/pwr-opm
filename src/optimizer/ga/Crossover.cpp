#include <random>

#include "../../lib/print.h"

#include "../GA.h"

namespace optimizer::ga::crossover {
  Crossover::Crossover(utils::Config *config, std::default_random_engine * e) {
    this->config = config;
    this->e = e;
  }

  SinglePointCrossover::SinglePointCrossover(utils::Config *config, std::default_random_engine * e) : Crossover(config, e) {}

  std::vector<problem::Solution *> SinglePointCrossover::run(problem::Solution *parentA, problem::Solution *parentB) {
    std::uniform_int_distribution<int> distribA(0, parentA->tour.size() - 1);
    std::uniform_int_distribution<int> distribB(0, parentB->tour.size() - 1);
    int crossoverPointA = distribA(*this->e);
    int crossoverPointB = distribB(*this->e);

    problem::Solution * childA = new problem::Solution(*parentA);
    problem::Solution * childB = new problem::Solution(*parentB);

    childA->tour.resize(crossoverPointA);
    childA->tour.insert(childA->tour.end(), std::next(parentB->tour.begin(), crossoverPointB), parentB->tour.end());

    childB->tour.resize(crossoverPointB);
    childB->tour.insert(childB->tour.end(), std::next(parentA->tour.begin(), crossoverPointA), parentA->tour.end());

    return {childA, childB};
  }  
}