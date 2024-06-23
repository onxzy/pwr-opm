#pragma once

#include "../lib/json.hpp"
using json = nlohmann::json;

namespace problem {
  namespace config {
    namespace fitness {
      struct GlobalFlat {
        float penalty = 0;
      };
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(GlobalFlat, penalty)

      struct GlobalWeights {
        float fitness = 1.0;
        float penalty = 1.0;
      };
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(GlobalWeights, fitness, penalty)

      struct Global {
        GlobalFlat flat;
        GlobalWeights weights;
      };
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Global, flat, weights)

      // fitness_reviews = sum_on_tour(reviews >= minReviews ? min(reviews, maxReviews) * note : 0)
      struct FitnessReviews {
        // Under this the reviews will not be taken into account
        // ie. fitness of 0 for this place
        int minReviews = 10;

        // Above this any more reviews will not be taken into account
        int maxReviews = 2000;
      };
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FitnessReviews, minReviews, maxReviews)

      struct FitnessWeights {
        float placesNb = 1;
        float price = 1;
        float reviews = 1;
      };
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FitnessWeights, placesNb, price, reviews)

      struct Fitness {
        FitnessWeights weights;
        FitnessReviews reviews;
      };
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Fitness, weights, reviews)

      struct PenaltyFlat {
        float unique = 5000;

        float breakfast = 500;
        float lunch = 500;
        float dinner = 500;
        float bar = 500;
      };
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
        PenaltyFlat,
        unique,
        breakfast,
        lunch,
        dinner,
        bar
      )

      struct PenaltyWeights {
        float placesOpen = 1.0;
        float totalDuration = 1.0;
        float maxPrice = 1.0;
        float breakfastStart = 1.0;
        float breakfastEnd = 1.0;
        float lunchStart = 1.0;
        float lunchEnd = 1.0;
        float dinnerStart = 1.0;
        float dinnerEnd = 1.0;
        float barStart = 1.0;
      };
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
        PenaltyWeights,
        placesOpen,
        totalDuration,
        maxPrice,
        breakfastStart,
        breakfastEnd,
        lunchStart,
        lunchEnd,
        dinnerStart,
        dinnerEnd,
        barStart
      )

      struct Penalty {
        bool enable = true;
        bool bonus = true;

        PenaltyFlat flat;
        PenaltyWeights weights;
      };
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Penalty, enable, bonus, flat, weights)
    }
    
    struct Fitness {
      fitness::Global global;
      fitness::Penalty penalty;
      fitness::Fitness fitness;
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Fitness, global, penalty, fitness)
  }

  struct ProblemConfig {
    config::Fitness fitness;
  };
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ProblemConfig, fitness)

}

namespace optimizer {
  namespace ga {
    namespace crossover {
      struct CrossoverConfig {
        float probability = 0.4f;
      };
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CrossoverConfig, probability)
    }

    namespace mutation {
      struct MutationConfig {
        float probability = 0.1f;
      };
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MutationConfig, probability)
    }

    namespace selection {
      namespace config {
        struct Tournament {
          int size = 3;
        };
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Tournament, size)
      }

      struct SelectionConfig {
        selection::config::Tournament tournament;
      };
      NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SelectionConfig, tournament)
    }

    struct GAConfig {
      int populationSize = 1000;
      int generations = 1000;

      selection::SelectionConfig selection;
      crossover::CrossoverConfig crossover;
      mutation::MutationConfig mutation;
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(GAConfig, populationSize, generations, selection, crossover, mutation)
  }

  namespace random {
    struct RandomConfig {
      int populationSize = 10000;
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RandomConfig, populationSize)
  }
  
  struct OptimizerConfig {
    ga::GAConfig ga;
    random::RandomConfig random;
  };
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(OptimizerConfig, ga, random)
}

namespace utils {
  struct Config {
    problem::ProblemConfig problem;
    optimizer::OptimizerConfig optimizer;
  };
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Config, problem, optimizer)


  Config loadConfig(std::string filename);
}
