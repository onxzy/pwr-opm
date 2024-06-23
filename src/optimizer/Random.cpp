#include "Random.h"

#include "../lib/print.h"

namespace optimizer::random {
  Random::Random(utils::Config * config, problem::Problem * problem, std::default_random_engine * e)
    : Optimizer(config, problem) {
    this->e = e;
    this->fitnessMap = new FitnessMap(config, &this->population);
  }

  problem::Solution * Random::run() {
    PRINTLN("Run : Random")

    this->population.clear();
    for (int i = 0; i < this->config->optimizer.random.populationSize; i++) {
      problem::Solution * solution = problem::Solution::random(this->problem, this->e);
      this->population.push_back(solution);
    }
    this->fitnessMap->evaluatePopulation();
    this->stats.computeStats(this->fitnessMap->getCurrentPopulationFitness());

    return this->fitnessMap->getBest(true);
  }
}