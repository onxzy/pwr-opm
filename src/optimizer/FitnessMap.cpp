#include "../lib/print.h"

#include "FitnessMap.h"

namespace optimizer {
  FitnessMap::FitnessMap(utils::Config * config, std::vector<problem::Solution *> * population) {
    this->config = config;
    this->population = population;
    this->map = new std::unordered_map<problem::Solution *, problem::SolutionFitness>();
  }

  void FitnessMap::evaluatePopulation() {
    for (auto it = this->population->begin(); it != this->population->end(); ++it) {
      // FIXME: Evaluate only if not already in map
      problem::Solution * solution = *it;
      problem::SolutionFitness fitness = solution->fitness(&this->config->problem.fitness);
      this->map->insert(std::make_pair(solution, fitness));
    }
  }

  problem::SolutionFitness * FitnessMap::getFitness(problem::Solution * solution) {
    auto it = this->map->find(solution);
    if (it != this->map->end()) {
      return &(*it).second;
    } else {
      problem::SolutionFitness fitness = solution->fitness(&this->config->problem.fitness);
      this->map->insert(std::make_pair(solution, fitness));
      return &this->map->at(solution);
    }
  }
}