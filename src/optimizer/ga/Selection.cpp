#include <random>

#include "../../lib/print.h"

#include "../GA.h"

namespace optimizer::ga::selection {
  Selection::Selection(utils::Config *config, std::default_random_engine * e) {
    this->config = config;
    this->e = e;
  }

  void Selection::init(FitnessMap * fitnessMap) {
    this->fitnessMap = fitnessMap;
  }

  RouletteWheelSelection::RouletteWheelSelection(utils::Config *config, std::default_random_engine * e) : Selection(config, e) {}

  problem::Solution *RouletteWheelSelection::run(std::vector<problem::Solution *> *population) {
    return nullptr;
  }

  TournamentSelection::TournamentSelection(utils::Config *config, std::default_random_engine * e) :
    Selection(config, e),
    size(config->optimizer.ga.selection.tournament.size)
    {}

  problem::Solution *TournamentSelection::run(std::vector<problem::Solution *> *population) {
    std::vector<problem::Solution *> tournament;
    std::uniform_int_distribution<int> distrib(0, population->size() - 1);

    for (int i = 0; i < this->size; i++) {
      tournament.push_back(population->at(distrib(*this->e)));
    }

    problem::Solution * winner = tournament.at(0);
    problem::SolutionFitness * bestFitness = this->fitnessMap->getFitness(winner);
    for (int i = 1; i < (int) tournament.size(); i++) {
      problem::SolutionFitness * fitness = this->fitnessMap->getFitness(tournament.at(i));
      if (*fitness < *bestFitness) {
        winner = tournament.at(i);
        bestFitness = fitness;
      }
    }

    return winner;
  }   
}