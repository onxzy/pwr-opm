#include "../lib/print.h"

#include "Solution.h"
#include "SolutionFitness.h"

#include <random>
#include <algorithm>
#include <unordered_set>

namespace problem {
  SolutionFitness Solution::computeFitness(config::Fitness * config) {
    SolutionFitness fitness = SolutionFitness();
    SolutionCompliant compliant = this->getCompliancy();
    bool isCompliant = compliant.summary();
    fitness.compliant = compliant;

    fitness.fitness += this->reviews(config) * config->fitness.weights.reviews;
    fitness.fitness += this->totalPrice() * config->fitness.weights.price;

    if (config->penalty.enable && !isCompliant) {
      fitness.penalty += compliant.placesUnique ? 0 : config->penalty.flat.unique;
      fitness.penalty += compliant.breakfast ? 0 : config->penalty.flat.breakfast;
      fitness.penalty += compliant.lunch ? 0 : config->penalty.flat.lunch;
      fitness.penalty += compliant.dinner ? 0 : config->penalty.flat.dinner;
      fitness.penalty += compliant.bar ? 0 : config->penalty.flat.bar;

      if (config->penalty.bonus) {
        fitness.penalty += compliant.placesOpen * config->penalty.weights.placesOpen;
        fitness.penalty += compliant.maxPrice * config->penalty.weights.maxPrice;
        fitness.penalty += compliant.totalDuration * config->penalty.weights.totalDuration;

        fitness.penalty += compliant.breakfastStart * config->penalty.weights.breakfastStart;
        fitness.penalty += compliant.breakfastEnd * config->penalty.weights.breakfastEnd;
        
        fitness.penalty += compliant.lunchStart * config->penalty.weights.lunchStart;
        fitness.penalty += compliant.lunchEnd * config->penalty.weights.lunchEnd;
 
        fitness.penalty += compliant.dinnerStart * config->penalty.weights.dinnerStart;
        fitness.penalty += compliant.dinnerEnd * config->penalty.weights.dinnerEnd;

        fitness.penalty += compliant.barStart * config->penalty.weights.barStart;
      } else {
        fitness.penalty += compliant.placesOpen >= 0 ? 0 : compliant.placesOpen * config->penalty.weights.placesOpen;
        fitness.penalty += compliant.maxPrice >= 0 ? 0 : compliant.maxPrice * config->penalty.weights.maxPrice;
        fitness.penalty += compliant.totalDuration >= 0 ? 0 : compliant.totalDuration * config->penalty.weights.totalDuration;

        fitness.penalty += compliant.breakfastStart >= 0 ? 0 : compliant.breakfastStart * config->penalty.weights.breakfastStart;
        fitness.penalty += compliant.breakfastEnd >= 0 ? 0 : compliant.breakfastEnd * config->penalty.weights.breakfastEnd;
        
        fitness.penalty += compliant.lunchStart >= 0 ? 0 : compliant.lunchStart * config->penalty.weights.lunchStart;
        fitness.penalty += compliant.lunchEnd >= 0 ? 0 : compliant.lunchEnd * config->penalty.weights.lunchEnd;

        fitness.penalty += compliant.dinnerStart >= 0 ? 0 : compliant.dinnerStart * config->penalty.weights.dinnerStart;
        fitness.penalty += compliant.dinnerEnd >= 0 ? 0 : compliant.dinnerEnd * config->penalty.weights.dinnerEnd;

        fitness.penalty += compliant.barStart >= 0 ? 0 : compliant.barStart * config->penalty.weights.barStart;
      }
    }

    fitness.global = fitness.fitness
      + ( 
        config->penalty.enable && !isCompliant
        ? (fitness.penalty + config->global.flat.penalty) * config->global.weights.penalty
        : 0
      );

    return fitness;
  }

  bool SolutionFitness::operator<(const SolutionFitness& other) const {
    return this->global < other.global;
  }

  bool SolutionFitness::operator>(const SolutionFitness& other) const {
    return this->global > other.global;
  }

  SolutionCompliant Solution::getCompliancy() {
    SolutionCompliant * result = new SolutionCompliant();
    problem::Constraints * constraints = this->problem->constraints;

    result->placesUnique = this->hasDuplicates();

    result->breakfast = !constraints->breakfast;
    result->lunch = !constraints->lunch;
    result->dinner = !constraints->dinner;
    result->bar = !constraints->bar;

    int t_finish = 0;

    auto loopFunction = [&](int i, int t_before, int t_in, int t_out, models::Place * place, TourPlaceType tourPlaceType) {
      switch (tourPlaceType) {
        case TourPlaceType::BREAKFAST:
          if (constraints->breakfast) {
            result->breakfast = true;
            result->breakfastStart = t_in - constraints->breakfastStart;
            result->breakfastEnd = constraints->breakfastEnd - t_out;
          }
          break;

        case TourPlaceType::LUNCH:
          if (constraints->lunch) {
            result->lunch = true;
            result->lunchStart = t_in - constraints->lunchStart;
            result->lunchEnd = constraints->lunchEnd - t_out;
          }
          break;

        case TourPlaceType::DINNER:
          if (constraints->dinner) {
            result->dinner = true;
            result->dinnerStart = t_in - constraints->dinnerStart;
            result->dinnerEnd = constraints->dinnerEnd - t_out;
          }
          break;

        case TourPlaceType::BAR:
          if (constraints->bar) {
            result->bar = true;
            result->barStart = t_in - constraints->barStart;
          }
          /* code */
          break;
      }

      if (t_in < place->open) {
        result->placesOpen -= place->open - t_in;
      }

      if (t_out > place->close) {
        result->placesOpen -= t_out - place->close;
      }
      
      t_finish = t_out;

      return false;
    };

    this->loopTour(loopFunction);

    result->totalDuration = constraints->totalDuration - this->totalTime();
    result->maxPrice = constraints->maxPrice > 0 ? constraints->maxPrice - this->totalPrice() : 0;

    return *result;
  }

  bool SolutionCompliant::summary() {
    return
      this->placesOpen >= 0 &&
      this->totalDuration >= 0 &&
      this->maxPrice >= 0 &&
      this->breakfast &&
      this->breakfastStart >= 0 &&
      this->breakfastEnd >= 0 &&
      this->lunch &&
      this->lunchStart >= 0 &&
      this->lunchEnd >= 0 &&
      this->dinner &&
      this->dinnerStart >= 0 &&
      this->dinnerEnd >= 0 &&
      this->bar &&
      this->barStart >= 0;
  }

  std::string SolutionCompliant::toString() {
    std::string result = "";

    result += this->placesOpen >= 0 ? "Places Open OK " : "Places Open NOK ";
    result += std::to_string(this->placesOpen) + "\n";

    result += this->totalDuration >= 0 ? "Total Duration OK " : "Total Duration NOK ";
    result += std::to_string(this->totalDuration) + "\n";
    result += this->maxPrice >= 0 ? "Max Price OK " : "Max Price NOK ";
    result += std::to_string(this->maxPrice) + "\n";

    result += this->breakfast ? "Breakfast OK\n" : "Breakfast NOK\n";
    result += this->breakfastStart >= 0 ? "Breakfast start OK " : "Breakfast start NOK ";
    result += std::to_string(this->breakfastStart) + "\n";
    result += this->breakfastEnd >= 0 ? "Breakfast end OK " : "Breakfast end NOK ";
    result += std::to_string(this->breakfastEnd) + "\n";

    result += this->lunch ? "Lunch OK\n" : "Lunch NOK\n";
    result += this->lunchStart >= 0 ? "Lunch start OK " : "Lunch start NOK ";
    result += std::to_string(this->lunchStart) + "\n";
    result += this->lunchEnd >= 0 ? "Lunch end OK " : "Lunch end NOK ";
    result += std::to_string(this->lunchEnd) + "\n";

    result += this->dinner ? "Dinner OK\n" : "Dinner NOK\n";
    result += this->dinnerStart >= 0 ? "Dinner start OK " : "Dinner start NOK ";
    result += std::to_string(this->dinnerStart) + "\n";
    result += this->dinnerEnd >= 0 ? "Dinner end OK " : "Dinner end NOK ";
    result += std::to_string(this->dinnerEnd) + "\n";

    result += this->bar ? "Bar OK\n" : "Bar NOK\n";
    result += this->barStart >= 0 ? "Bar start OK " : "Bar start NOK ";
    result += std::to_string(this->barStart) + "\n";

    return result;
  }
}