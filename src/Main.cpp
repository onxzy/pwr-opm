#include "Defaults.h"

#include <iostream>
#include <exception>
#include <string>
#include <ctime>

#include <boost/program_options.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>
using namespace boost::numeric::ublas;
namespace po = boost::program_options;

#include "lib/json.hpp"
using json = nlohmann::json;

#include "utils/Config.h"
#include "utils/Stats.h"
using namespace utils;

#include "problem/Problem.h"
#include "problem/Solution.h"
using namespace problem;

#include "optimizer/GA.h"
#include "optimizer/Random.h"
using namespace optimizer;

void onterminate() {
  try {
    auto unknown = std::current_exception();
    if (unknown) {
      std::rethrow_exception(unknown);
    } else {
      std::cerr << "normal termination" << std::endl;
    }
  } catch (const std::exception& e) { // for proper `std::` exceptions
    std::cerr << "unexpected exception: " << e.what() << std::endl;
  } catch (...) { // last resort for things like `throw 1;`
    std::cerr << "unknown exception" << std::endl;
  }
}

int main(int argc, char *argv[]) {
  std::set_terminate(onterminate);

  po::options_description cmd_options("global options");
  cmd_options.add_options()
    ("help,h", "display help")
    ("seed", po::value<int>(), "random engine seed")
    ("config", po::value<std::string>()->default_value((std::string) "config/default.json"), "config file")
    ("input", po::value<std::string>()->required(), "output folder")
    ("max-problem-nb", po::value<int>()->default_value(-1), "number of problems to load")
    ("optimizer,o", po::value<std::string>()->default_value((std::string) "random"), "random, ga")
  ;

  po::variables_map vm;
  po::parsed_options parsed = po::command_line_parser(argc, argv)
    .options(cmd_options)
    .allow_unregistered()
    .run();
  po::store(parsed, vm);

  if (vm.count("help")) {
    PRINTLN(cmd_options);
    return 0;
  }

  Config config = loadConfig(vm["config"].as<std::string>());
  int seed = vm.count("seed") ? vm["seed"].as<int>() : (int) std::time(nullptr);

  std::vector<Problem> problems = Problem::loadBulk(vm["input"].as<std::string>(), vm["max-problem-nb"].as<int>());

  PRINTLN(problems.size() << " problems loaded");

  std::default_random_engine e(seed);

  // Solution * solution = Solution::random(&problems.back(), &e);
  // SolutionFitness fitness = solution->computeFitness(&config.problem.fitness);
  // std::cout << solution->toString() << std::endl;
  // std::cout << "===================================================" << std::endl;
  // std::cout << "Global :" << fitness.global << std::endl;
  // std::cout << "Fitness :" << fitness.fitness << std::endl;
  // std::cout << "Compliant" << std::endl;
  // std::cout << fitness.compliant.toString() << std::endl;

  // return 0;

  Optimizer * optimizer = nullptr;

  std::string optimizerName = vm["optimizer"].as<std::string>();

  if (optimizerName == "random") {
    optimizer = new random::Random(&config, &problems.back(), &e);
  } else if (optimizerName == "ga") {
    ga::selection::Selection * selection = new ga::selection::TournamentSelection(&config, &e);
    ga::crossover::Crossover * crossover = new ga::crossover::SinglePointCrossover(&config, &e);
    ga::mutation::Mutation * mutation = new ga::mutation::SwapMutation(&config, &e);

    optimizer = new ga::GA(&config, &problems.back(), &e, selection, crossover, mutation);
  } else {
    std::cerr << "Optimizer " << optimizerName << " unknown" << std::endl;
    return 1;
  }

  Solution * best = optimizer->run();
  std::cout << "===================================================" << std::endl;
  std::cout << optimizer->getStats()->dumpCsv() << std::endl;
  std::cout << "===================================================" << std::endl;

  if (best == nullptr) {
    std::cerr << "No solution found" << std::endl;
    return 1;
  }
  SolutionFitness bestFitness = best->computeFitness(&config.problem.fitness);

  std::cout << "===================================================" << std::endl;
  std::cout << best->toString() << std::endl;
  std::cout << "===================================================" << std::endl;
  std::cout << "Global : " << bestFitness.global << std::endl;
  std::cout << "Fitness : " << bestFitness.fitness << std::endl;
  std::cout << "Penalty : " << bestFitness.penalty << std::endl;
  std::cout << "Compliant : " << std::to_string(bestFitness.compliant.summary()) << std::endl;
  std::cout << bestFitness.compliant.toString() << std::endl;

  return 0;
}