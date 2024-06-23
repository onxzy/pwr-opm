#include "../lib/print.h"

#include "FitnessMap.h"

namespace optimizer {
  FitnessMap::FitnessMap(utils::Config * config, std::vector<problem::Solution *> * population) {
    this->config = config;
    this->population = population;
    this->map = new std::unordered_map<problem::Solution *, problem::SolutionFitness>();
  }

  void FitnessMap::init() {
    this->map->clear();
  }

  void FitnessMap::evaluatePopulation() {
    // if (this->map->size() > 100000) {
    //   std::cout << "Clear map" << std::endl;
    //   this->map->clear();
    // }

    for (auto it = this->population->begin(); it != this->population->end(); ++it) {
      problem::Solution * solution = *it;
      if (this->map->find(solution) == this->map->end()) continue;

      problem::SolutionFitness fitness = solution->computeFitness(&this->config->problem.fitness);
      this->map->insert(std::make_pair(solution, fitness));
    }
  }

  problem::SolutionFitness * FitnessMap::getFitness(problem::Solution * solution, bool force) {
    auto it = this->map->find(solution);
    if (it != this->map->end()) {
      if (force) {
        problem::SolutionFitness fitness = solution->computeFitness(&this->config->problem.fitness);
        it->second = fitness;
      }
      return &it->second;
    } else {
      problem::SolutionFitness fitness = solution->computeFitness(&this->config->problem.fitness);
      this->map->insert(std::make_pair(solution, fitness));
      return &this->map->at(solution);
    }
  }

  std::vector<problem::SolutionFitness> FitnessMap::getCurrentPopulationFitness() {
    std::vector<problem::SolutionFitness> fitness;
    for (auto it = this->population->begin(); it!= this->population->end(); ++it) {
      fitness.push_back(*this->getFitness(*it));
    }
    return fitness;
  }

  problem::Solution * FitnessMap::getBest(bool compliant) {
    this->evaluatePopulation();

    problem::Solution * best = nullptr;
    problem::SolutionFitness * bestFitness = nullptr;

    // Loop over map
    for (auto it = this->map->begin(); it!= this->map->end(); ++it) {
      problem::Solution * solution = (*it).first;
      problem::SolutionFitness * fitness = &(*it).second;

      if (compliant && !fitness->compliant.summary()) continue;

      if (best == nullptr || *fitness > *bestFitness) {
        best = solution;
        bestFitness = fitness;
      }
    }

    if (compliant && best == nullptr) return this->getBest(false);

    return best;
  }
}