#pragma once

#include "../utils/Config.h"
#include "../problem/Solution.h"

namespace optimizer {
  class FitnessMap {
    public:
      FitnessMap(utils::Config * config, std::vector<problem::Solution *> * population);
      void init();
      void evaluatePopulation();
      problem::SolutionFitness * getFitness(problem::Solution * solution, bool force = false);
      std::vector<problem::SolutionFitness> getCurrentPopulationFitness();
      problem::Solution * getBest(bool compliant = true);

    private:
      utils::Config * config;
      std::vector<problem::Solution *> * population;
      std::unordered_map<problem::Solution *, problem::SolutionFitness> * map;
  };
}