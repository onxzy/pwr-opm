#include "Stats.h"

namespace utils {

  void Stats::printStats(std::vector<problem::SolutionFitness> fitness) {
    // Get max of fitness.global
    auto max = std::max_element(fitness.begin(), fitness.end(), [](problem::SolutionFitness a, problem::SolutionFitness b) {
      return a.global < b.global;
    });

    // Get min of fitness.global
    auto min = std::min_element(fitness.begin(), fitness.end(), [](problem::SolutionFitness a, problem::SolutionFitness b) {
      return a.global < b.global;
    });

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
    auto nb_compliant = std::count_if(fitness.begin(), fitness.end(), [](problem::SolutionFitness a) {
      return a.penalty == 0;
    });

    // Print all stats
    std::cout << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "Number of solutions: " << fitness.size() << std::endl;
    std::cout << "Max global: " << max->global << std::endl;
    std::cout << "Min global: " << min->global << std::endl;
    std::cout << "Max fitness: " << max_fitness->fitness << std::endl;
    std::cout << "Min fitness: " << min_fitness->fitness << std::endl;
    std::cout << "Max penalty: " << max_penalty->penalty << std::endl;
    std::cout << "Min penalty: " << min_penalty->penalty << std::endl;
    std::cout << "Number of compliant: " << nb_compliant << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;

  }

}