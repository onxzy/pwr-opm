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

#include "problem/Problem.h"
#include "problem/Solution.h"
#include "utils/Stats.h"
#include "utils/Config.h"
using namespace models;
using namespace problem;
using namespace utils;

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
    ("output", po::value<std::string>()->default_value((std::string) "problems"), "output folder")
    ("number,n", po::value<int>()->default_value(10), "Number of problems to generate")
    ("places,p", po::value<int>()->default_value(100), "Number of places to generate")
    ("city-size,c", po::value<int>()->default_value(500), "Size of the city")
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

  Problem::generateBulk(vm["output"].as<std::string>(), vm["number"].as<int>(), vm["places"].as<int>(), vm["city-size"].as<int>(), seed);

  return 0;
}