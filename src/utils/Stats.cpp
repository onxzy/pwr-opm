#include "Stats.h"

namespace utils {

  void Stats::computeStats(std::vector<problem::SolutionFitness> fitnesses) {
    stats::PopulationStats stats = {
      fitnesses.size(),
      Stats::nbCompliant(fitnesses),
      Stats::min(fitnesses),
      Stats::max(fitnesses),
      Stats::mean(fitnesses),
      Stats::std(fitnesses)
    };

    this->populationStats.push_back(stats);
  }

  std::string Stats::dumpCsv() {
    std::string csv = "";
    csv += "size,nbCompliant,min,max,mean,std\n";
    for (auto stats : this->populationStats) {
      csv += 
        std::to_string(stats.size) + "," +
        std::to_string(stats.nbCompliant) + "," +
        std::to_string(stats.min) + "," +
        std::to_string(stats.max) + "," +
        std::to_string(stats.mean) + "," +
        std::to_string(stats.std) + "\n";
    }
    return csv;
  }

  void Stats::printStats(std::vector<problem::SolutionFitness> fitness) {
    // Get max of fitness.global
    auto max = Stats::max(fitness);

    // Get min of fitness.global
    auto min = Stats::min(fitness);

    // Get max of fitness.fitness
    auto max_fitness = std::max_element(fitness.begin(), fitness.end(), [](problem::SolutionFitness a, problem::SolutionFitness b) {
      return a.fitness < b.fitness;
    });

    // Get min of fitness.fitness
    auto min_fitness = std::min_element(fitness.begin(), fitness.end(), [](problem::SolutionFitness a, problem::SolutionFitness b) {
      return a.fitness < b.fitness;
    });

    // Get max of fitness.penalty
    auto max_penalty = std::max_element(fitness.begin(), fitness.end(), [](problem::SolutionFitness a, problem::SolutionFitness b) {
      return a.penalty < b.penalty;
    });

    // Get min of fitness.penalty
    auto min_penalty = std::min_element(fitness.begin(), fitness.end(), [](problem::SolutionFitness a, problem::SolutionFitness b) {
      return a.penalty < b.penalty;
    });

    // Get number of compliant
    auto nb_compliant = Stats::nbCompliant(fitness);

    // Print all stats
    std::cout << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "Number of solutions: " << fitness.size() << std::endl;
    std::cout << "Max global: " << max << std::endl;
    std::cout << "Min global: " << min << std::endl;
    std::cout << "Max fitness: " << max_fitness->fitness << std::endl;
    std::cout << "Min fitness: " << min_fitness->fitness << std::endl;
    std::cout << "Max penalty: " << max_penalty->penalty << std::endl;
    std::cout << "Min penalty: " << min_penalty->penalty << std::endl;
    std::cout << "Number of compliant: " << nb_compliant << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;

  }

  float Stats::min(std::vector<problem::SolutionFitness> fitnesses) {
    return std::min_element(fitnesses.begin(), fitnesses.end(), [](problem::SolutionFitness a, problem::SolutionFitness b) {
      return a.global < b.global;
    })->global;
  }

  float Stats::max(std::vector<problem::SolutionFitness> fitnesses) {
    return std::max_element(fitnesses.begin(), fitnesses.end(), [](problem::SolutionFitness a, problem::SolutionFitness b) {
      return a.global < b.global;
    })->global;
  }

  float Stats::mean(std::vector<problem::SolutionFitness> fitnesses) {
    float sum = 0;
    for (auto fitness : fitnesses) {
      sum += fitness.global;
    }
    return sum / fitnesses.size();
  }

  float Stats::std(std::vector<problem::SolutionFitness> fitnesses) {
    float mean = Stats::mean(fitnesses);
    float sum = 0;
    for (auto fitness : fitnesses) {
      sum += (fitness.global - mean) * (fitness.global - mean);
    }
    return sqrt(sum / fitnesses.size());
  }

  int Stats::nbCompliant(std::vector<problem::SolutionFitness> fitnesses) {
    return std::count_if(fitnesses.begin(), fitnesses.end(), [](problem::SolutionFitness a) {
      return a.compliant.summary();
    });
  }

}