#include <random>

#include "GA.h"
#include "../lib/print.h"

#include "../problem/Solution.h"

#include "FitnessMap.h"

namespace optimizer {
  namespace ga {
    GA::GA(utils::Config * config, problem::Problem * problem, std::default_random_engine * e, selection::Selection * selection, crossover::Crossover * crossover, mutation::Mutation * mutation)
      : Optimizer(config, problem) {
      this->e = e;
      this->fitnessMap = new FitnessMap(config, &this->population);

      this->selection = selection;
      this->selection->init(this->fitnessMap);

      this->crossover = crossover;
      this->mutation = mutation;
    }

    void GA::initPopulation() {
      for (int i = 0; i < this->config->optimizer.ga.populationSize; i++) {
        problem::Solution * solution = problem::Solution::random(this->problem, this->e);
        this->population.push_back(solution);
      }
    }

    void GA::run() {
      PRINTLN("Run GA");

      this->initPopulation();

      this->fitnessMap->evaluatePopulation();

      // problem::SolutionFitness * fitA = this->fitnessMap->getFitness(*this->population.begin());
      // problem::Solution * sol2 = problem::Solution::random(this->problem, this->e);
      // problem::SolutionFitness * fitB = this->fitnessMap->getFitness(sol2);
      // problem::SolutionFitness * fitC = this->fitnessMap->getFitness(sol2);

      problem::Solution * parentA = this->selection->run(&this->population);
      problem::Solution * parentB = parentA;
      while (parentA == parentB) parentB = this->selection->run(&this->population);

      std::vector<problem::Solution *> childs = this->crossover->run(parentA, parentB);


      // this->crossover->run(nullptr, nullptr);
      // this->mutation->run(nullptr);
    }
  }
}