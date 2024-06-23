#pragma once

#include <random>

#include "Optimizer.h"
#include "FitnessMap.h"

#include "../problem/Solution.h"

namespace optimizer::ga {
  namespace selection {
    class Selection {
      public:
        Selection(utils::Config * config, std::default_random_engine * e);
        void init(FitnessMap * fitnessMap);
        virtual problem::Solution * run(std::vector<problem::Solution *> * population) = 0;

      protected:
        utils::Config * config;
        std::default_random_engine * e;
        FitnessMap * fitnessMap;
    };

    class RouletteWheelSelection : public Selection {
      public:
        RouletteWheelSelection(utils::Config * config, std::default_random_engine * e);
        problem::Solution * run(std::vector<problem::Solution *> * population);
    };

    class TournamentSelection : public Selection {
      public:
        TournamentSelection(utils::Config * config, std::default_random_engine * e);
        problem::Solution * run(std::vector<problem::Solution *> * population);

      private:
        const int size;
    };
  }

  namespace crossover {
    class Crossover {
      public:
        Crossover(utils::Config * config, std::default_random_engine * e);

        virtual std::vector<problem::Solution *> run(problem::Solution * parentA, problem::Solution * parentB) = 0;

      protected:
        utils::Config * config;
        std::default_random_engine * e;
    };

    class SinglePointCrossover : public Crossover {
      public:
        SinglePointCrossover(utils::Config * config, std::default_random_engine * e);
        std::vector<problem::Solution *> run(problem::Solution * parentA, problem::Solution * parentB);
    };
  }

  namespace mutation {
    class Mutation {
      public:
        Mutation(utils::Config * config, std::default_random_engine * e);
        virtual void run(problem::Solution * solution) = 0;

      protected:
        utils::Config * config;
        std::default_random_engine * e;
    };

    class SwapMutation : public Mutation {
      public:
        SwapMutation(utils::Config * config, std::default_random_engine * e);
        void run(problem::Solution * solution);
    };
  }

  class GA: public Optimizer {
    public:
      GA(utils::Config * config, problem::Problem * problem, std::default_random_engine * e, selection::Selection * seclection, crossover::Crossover * crossover, mutation::Mutation * mutation);
      problem::Solution * run();

    private:
      std::default_random_engine * e;

      crossover::Crossover * crossover;
      mutation::Mutation * mutation;
      selection::Selection * selection;

      std::vector<problem::Solution *> population;

      FitnessMap * fitnessMap;

      void initPopulation();
      void iteration();
  };
}

