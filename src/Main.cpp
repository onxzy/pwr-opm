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

  ga::selection::Selection * selection = new ga::selection::TournamentSelection(&config, &e);
  ga::crossover::Crossover * crossover = new ga::crossover::SinglePointCrossover(&config, &e);
  ga::mutation::Mutation * mutation = new ga::mutation::SwapMutation(&config, &e);

  Optimizer * optimizer = new ga::GA(&config, &problems.back(), &e, selection, crossover, mutation);

  optimizer->run();

  return 0;
}