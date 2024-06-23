#pragma once

#include "../problem/Problem.h"
#include "../problem/Solution.h"

namespace utils {
  namespace stats {
    struct PopulationStats {
      int size;
      int nbCompliant;
      float min;
      float max;
      float mean;
      float std;
    };
  }

  class Stats {
    public:
      static void printStats(std::vector<problem::SolutionFitness> fitnesses);
      void computeStats(std::vector<problem::SolutionFitness> fitnesses);
      std::string dumpCsv();

    private:
      std::list<stats::PopulationStats> populationStats;

      static float min(std::vector<problem::SolutionFitness> fitnesses);
      static float max(std::vector<problem::SolutionFitness> fitnesses);
      static float mean(std::vector<problem::SolutionFitness> fitnesses);
      static int nbCompliant(std::vector<problem::SolutionFitness> fitnesses);
      static float std(std::vector<problem::SolutionFitness> fitnesses);
  };
}
