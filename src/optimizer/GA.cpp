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
      this->population.clear();
      for (int i = 0; i < this->config->optimizer.ga.populationSize; i++) {
        problem::Solution * solution = problem::Solution::random(this->problem, this->e);
        this->population.push_back(solution);
      }
      this->fitnessMap->evaluatePopulation();
      this->stats.computeStats(this->fitnessMap->getCurrentPopulationFitness());
    }

    problem::Solution * GA::run() {
      PRINTLN("Run : GA")

      this->fitnessMap->init();
      this->initPopulation();

      for (int i = 0; i < this->config->optimizer.ga.generations; i++) {
        this->iteration();
        if (i % 10 == 0) std::cout << i << "/" << this->config->optimizer.ga.generations << std::endl;
      }

      return this->fitnessMap->getBest(true);
    }

    void GA::iteration() {
      std::vector<problem::Solution *> newPopulation;

      while ((int) newPopulation.size() < this->config->optimizer.ga.populationSize) {
        problem::Solution * parentA = this->selection->run(&this->population);
        problem::Solution * parentB;
        do {
          parentB = this->selection->run(&this->population);
        } while (parentA == parentB);
        std::vector<problem::Solution *> childs = this->crossover->run(parentA, parentB);
        newPopulation.insert(newPopulation.end(), childs.begin(), childs.end());
      }

      for (int i = 0; i < (int) newPopulation.size(); i++) {
        this->mutation->run(newPopulation[i]);
      }

      for (int i = 0; i < (int) newPopulation.size(); i++) {
        problem::Solution * element =  newPopulation[i];

        problem::SolutionCompliant compliant = element->getCompliancy();
        element->repair(&compliant, this->e);

        if (this->fitnessMap->getFitness(element, true)->compliant.totalDuration > -2*60) continue;

        PRINTLN("Discard")
        newPopulation[i] = problem::Solution::random(this->problem, this->e);
      }

      this->fitnessMap->evaluatePopulation();

      this->population.clear();
      this->population.insert(this->population.end(), newPopulation.begin(), newPopulation.end());

      this->stats.computeStats(this->fitnessMap->getCurrentPopulationFitness());
    }
  }
}