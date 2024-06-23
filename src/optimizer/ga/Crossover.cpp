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
    if (std::uniform_real_distribution<float>(0, 1)(*this->e) > this->config->optimizer.ga.crossover.probability) {
      return {parentA, parentB};
    }

    int crossoverPointA = std::uniform_int_distribution<int>(0, parentA->tour.size() - 1)(*this->e);
    int crossoverPointB = std::uniform_int_distribution<int>(0, parentB->tour.size() - 1)(*this->e);

    problem::Solution * childA = new problem::Solution(*parentA);
    problem::Solution * childB = new problem::Solution(*parentB);

    std::list<models::Place *> bufferA;
    bufferA.splice(bufferA.begin(), childA->tour, std::next(childA->tour.begin(), crossoverPointA), childA->tour.end());
    // childA = childA[0:crossoverPointA)
    // bufferA = childA[crossoverPointA:]

    std::list<models::Place *> bufferB;
    bufferB.splice(bufferB.begin(), childB->tour, std::next(childB->tour.begin(), crossoverPointB), childB->tour.end());
    // childB = childB[0:crossoverPointB)
    // bufferB = childB[crossoverPointB:]

    childA->tour.splice(childA->tour.end(), bufferB);
    childB->tour.splice(childB->tour.end(), bufferA);

    return {childA, childB};
  }  
}