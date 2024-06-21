#include <fstream>

#include "../lib/json.hpp"
using json = nlohmann::json;

#include "Config.h"

namespace utils {
  Config loadConfig(std::string filename) {
    std::ifstream inputFile(filename);
    json j = json::parse(inputFile);
    return j.template get<Config>();
  }
}