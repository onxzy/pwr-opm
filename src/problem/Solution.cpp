#include "../lib/print.h"

#include "Solution.h"

#include <random>
#include <algorithm>
#include <unordered_set>

namespace problem {
  Solution::Solution(Problem* problem) {
    this->problem = problem;
  }

  int Solution::currentTravelTimeToPlace(models::Place * nextPlace) {
    if (this->tour.size() > 0) {
      return this->problem->getTravelTime(
          &this->tour.back()->locationB, 
          &nextPlace->locationA
        );
    } else if (this->problem->constraints->start) {
      return this->problem->getTravelTime(
          this->problem->constraints->start, 
          &nextPlace->locationA
        );
    } else return 0;
  }

  void Solution::loopTour(std::function<bool(int i, int t_before, int t_in, int t_out, models::Place * place, TourPlaceType tourPlaceType)> lambda) {
    int t = this->problem->constraints->startTime;

    bool breakfastDone = false;
    bool lunchDone = false;
    bool dinnerDone = false;
    bool barDone = false;

    int i = 0;
    for (auto it = this->tour.begin(); it != this->tour.end(); ++it) {
      models::Place * place = *it;

      int t_before = t;

      if (i) {
        t += this->problem->getTravelTime(
          &(*std::prev(it, 1))->locationB,
          &place->locationA
        );
      } else if (this->problem->constraints->start) {
        t += this->problem->getTravelTime(
          this->problem->constraints->start, 
          &place->locationA
        );
      }

      bool breakLoop = false;

      if (place->type == models::PlaceType::cafe && this->problem->constraints->breakfast && !breakfastDone) {
        // Wait until breakfast time
        if (t < this->problem->constraints->breakfastStart) t = this->problem->constraints->breakfastStart;
        
        // Wait until it opens
        if (t < place->open) t = place->open;

        int t_in = t;

        t += this->problem->constraints->breakfastDuration;
        breakfastDone = true;
        breakLoop = lambda(i, t_before, t_in, t, place, TourPlaceType::BREAKFAST);
      
      } else if (place->type == models::PlaceType::restaurant && this->problem->constraints->lunch && !lunchDone) {
        // Wait until lunch time
        if (t < this->problem->constraints->lunchStart) t = this->problem->constraints->lunchStart;
        
        // Wait until it opens
        if (t < place->open) t = place->open;

        int t_in = t;

        t += this->problem->constraints->lunchDuration;
        lunchDone = true;
        breakLoop = lambda(i, t_before, t_in, t, place, TourPlaceType::LUNCH);
      
      } else if (place->type == models::PlaceType::restaurant && this->problem->constraints->dinner && !dinnerDone) {
        // Wait until dinner time
        if (t < this->problem->constraints->dinnerStart) t = this->problem->constraints->dinnerStart;
        
        // Wait until it opens
        if (t < place->open) t = place->open;

        int t_in = t;

        t += this->problem->constraints->dinnerDuration;
        dinnerDone = true;
        breakLoop = lambda(i, t_before, t_in, t, place, TourPlaceType::DINNER);
      
      } else if (place->type == models::PlaceType::bar && this->problem->constraints->bar && !barDone) {
        // Wait until bar time
        if (t < this->problem->constraints->barStart) t = this->problem->constraints->barStart;
        
        // Wait until it opens
        if (t < place->open) t = place->open;

        int t_in = t;

        t += this->problem->constraints->barDuration;
        barDone = true;
        breakLoop = lambda(i, t_before, t_in, t, place, TourPlaceType::BAR);
      
      } else {
        // Wait until it opens
        if (t < place->open) t = place->open;

        int t_in = t;

        t += place->timeToVisit;
        breakLoop = lambda(i, t_before, t_in, t, place, TourPlaceType::ATTRACTION);
      }

      if (breakLoop) break;

      i++;
    }
  }

  std::string Solution::toString() {
    std::string result = "";

    auto loopFunction = [&result](int i, int t_before, int t_in, int t_out, models::Place * place, TourPlaceType _) {
      result += place->typeToString();
      result += " " + place->name + " @[" + std::to_string(t_in) + ":" + std::to_string(t_out) + "] (" + std::to_string(place->open) + ":" + std::to_string(place->close) + ")\n";
      return false;
    };

    this->loopTour(loopFunction);

    return result;
  }

  SolutionFitness Solution::fitness(config::Fitness * config) {
    SolutionFitness fitness = SolutionFitness();
    SolutionCompliant compliant = this->compliant();
    bool isCompliant = compliant.summary();
    fitness.compliant = compliant;

    fitness.fitness += this->reviews(config) * config->fitness.weights.reviews;
    fitness.fitness += this->totalPrice() * config->fitness.weights.price;

    if (config->penalty.enable && !isCompliant) {
      fitness.penalty += compliant.placesOpen * config->penalty.weights.placesOpen;
      fitness.penalty += compliant.maxPrice * config->penalty.weights.maxPrice;
      fitness.penalty += compliant.totalDuration * config->penalty.weights.totalDuration;

      fitness.penalty += compliant.placesUnique ? 0 : config->penalty.flat.unique;
      fitness.penalty += compliant.breakfast ? 0 : config->penalty.flat.breakfast;
      fitness.penalty += compliant.lunch ? 0 : config->penalty.flat.lunch;
      fitness.penalty += compliant.dinner ? 0 : config->penalty.flat.dinner;
      fitness.penalty += compliant.bar ? 0 : config->penalty.flat.bar;

      if (config->penalty.bonus) {
        fitness.penalty += compliant.breakfastStart * config->penalty.weights.breakfastStart;
        fitness.penalty += compliant.breakfastEnd * config->penalty.weights.breakfastEnd;
        
        fitness.penalty += compliant.lunchStart * config->penalty.weights.lunchStart;
        fitness.penalty += compliant.lunchEnd * config->penalty.weights.lunchEnd;
 
        fitness.penalty += compliant.dinnerStart * config->penalty.weights.dinnerStart;
        fitness.penalty += compliant.dinnerEnd * config->penalty.weights.dinnerEnd;

        fitness.penalty += compliant.barStart * config->penalty.weights.barStart;
      } else {
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

  SolutionCompliant Solution::compliant() {
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
    result->maxPrice = constraints->maxPrice > 0 ? constraints->maxPrice - this->totalPrice()  : 0;

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

  models::Place* Solution::pickPlace(int * t, std::vector<models::Place *>* places, int overrideDuration) {

    models::Place * foundPlace = nullptr;

    for (auto it = places->begin(); it != places->end(); ++it) {

      // std::cout << (*it)->name << ": " << this->currentTravelTimeToPlace(*it);
      // std::cout << " " << std::to_string((overrideDuration > 0) ? overrideDuration : (*it)->timeToVisit);
      // std::cout << " (" << overrideDuration;
      // std::cout << "-" << (*it)->timeToVisit;
      // std::cout << ")";
      // std::cout << " " << (*it)->close;
      // std::cout << std::endl;

      int t_in = *t + this->currentTravelTimeToPlace(*it);
      int t_out = t_in + (((bool) overrideDuration) ? overrideDuration : (*it)->timeToVisit);

      if ((*it)->open <= t_in && t_out <= (*it)->close) {
        foundPlace = *it;
        places->erase(it);
        break;
      }
    }

    if (foundPlace != nullptr) {
      *t += this->currentTravelTimeToPlace(foundPlace);

      // std::cout << foundPlace->typeToString() << " " + foundPlace->name << " [" << *t << ":";
      *t += overrideDuration ? overrideDuration : foundPlace->timeToVisit;
      // std::cout << *t << "]" << std::endl;      

      this->tour.push_back(foundPlace);
    }

    return foundPlace;
  }

  Solution * Solution::random(Problem* problem, std::default_random_engine* randomEngine) {
    Solution * solution = new Solution(problem);

    std::vector<models::Place *> attractions = problem->findPlacesByType(models::PlaceType::attraction);
    std::vector<models::Place *> restaurants = problem->findPlacesByType(models::PlaceType::restaurant);
    std::vector<models::Place *> bars = problem->findPlacesByType(models::PlaceType::bar);
    std::vector<models::Place *> cafes = problem->findPlacesByType(models::PlaceType::cafe);

    std::shuffle(attractions.begin(), attractions.end(), *randomEngine);
    std::shuffle(restaurants.begin(), restaurants.end(), *randomEngine);
    std::shuffle(bars.begin(), bars.end(), *randomEngine);
    std::shuffle(cafes.begin(), cafes.end(), *randomEngine);

    models::Place * breakfast = nullptr;
    models::Place * lunch = nullptr;
    models::Place * dinner = nullptr;
    models::Place * bar = nullptr;

    const int t_start = problem->constraints->startTime;
    int t = t_start;

    int it_counter = 0;

    while (t < t_start + problem->constraints->totalDuration) {
      it_counter++;

      // PRINTLN(t);

      // Find Breakfast
      if (problem->constraints->breakfast &&
          problem->constraints->breakfastStart <= t && // t <= problem->constraints->breakfastEnd &&
          breakfast == nullptr &&
          cafes.size()) {

        // PRINTLN("Breakfast");
        
        breakfast = solution->pickPlace(&t, &cafes, problem->constraints->breakfastDuration);

        if (breakfast != nullptr) continue;
      }

      // Find Lunch
      if (problem->constraints->lunch &&
          problem->constraints->lunchStart <= t && // t <= problem->constraints->lunchEnd &&
          lunch == nullptr &&
          restaurants.size()) {

        // PRINTLN("Lunch");

        lunch = solution->pickPlace(&t, &restaurants, problem->constraints->lunchDuration);

        if (lunch != nullptr) continue;
      }

      // Find Dinner
      if (problem->constraints->dinner &&
          problem->constraints->dinnerStart <= t && // t <= problem->constraints->dinnerEnd &&
          dinner == nullptr &&
          restaurants.size()) {

        // PRINTLN("Dinner");

        dinner = solution->pickPlace(&t, &restaurants, problem->constraints->dinnerDuration);

        if (dinner != nullptr) continue;
      }

      // Find Bar
      if (problem->constraints->bar &&
          problem->constraints->barStart <= t &&
          bar == nullptr &&
          bars.size()) {

        // PRINTLN("Bar");

        bar = solution->pickPlace(&t, &bars, problem->constraints->barDuration);

        if (bar != nullptr) continue;
      }

      // Stop looking for new places when we are at the bar = end of tour
      if (problem->constraints->bar && bar != nullptr) break;

      // Find attraction
      if (attractions.size()) {

        models::Place * foundAttraction = solution->pickPlace(&t, &attractions);

        if (foundAttraction != nullptr) continue;
      }
      
      // By default advance by 10 minutes
      t += 10;
      // PRINTLN("Default");
    }

    // std::cout << "it " << it_counter << std::endl;

    return solution;
  }

  int Solution::travelTime() {
    int time = 0;

    time += this->problem->placesTravelTimes (this->problem->constraints->start->index, (*this->tour.begin())->locationA.index);

    for (auto it = this->tour.begin(); it != this->tour.end(); ++it){
      if (std::next(it) != this->tour.end()) {
        time += this->problem->placesTravelTimes ((*it)->locationB.index, (*std::next(it))->locationA.index);
      }
    }

    if (this->problem->constraints->end != nullptr)
      time += this->problem->placesTravelTimes (this->tour.back()->locationB.index, this->problem->constraints->end->index);

    return time;
  }

  int Solution::totalTime() {
    int time = 0;

    auto loopFunction = [&](int i, int t_before, int t_in, int t_out, models::Place * place, TourPlaceType _) {
      time = t_out;
      return false;
    };

    this->loopTour(loopFunction);

    if (this->problem->constraints->end != nullptr)
      time += this->problem->placesTravelTimes (this->tour.back()->locationB.index, this->problem->constraints->end->index);

    return time - this->problem->constraints->startTime;
  }

  int Solution::totalPrice() {
    int price = 0;

    for (auto it = this->tour.begin(); it != this->tour.end(); ++it) {
      price += (*it)->price;
    }

    return price;
  }

  float Solution::reviews(config::Fitness * config) {
    float result = 0;

    for (auto it = this->tour.begin(); it != this->tour.end(); ++it) {
      if ((*it)->reviews < config->fitness.reviews.minReviews) continue;
      result += std::min(config->fitness.reviews.maxReviews, (*it)->reviews) * (*it)->note;
    }

    return result;
  }

  int Solution::numberOfPlaces() {
    return this->tour.size();
  }

  bool Solution::hasDuplicates() {
    std::unordered_set<models::Place *> set;

    for (auto it = this->tour.begin(); it!= this->tour.end(); ++it) {
      set.insert(*it);
    }

    return set.size() < this->tour.size();
  }

  void Solution::removeDuplicates() {
    std::set<models::Place *> set;
    for (auto it = this->tour.begin(); it!= this->tour.end(); ++it) {
      set.insert(*it);
    }

    this->tour.clear();
    for (auto it = set.begin(); it != set.end(); ++it) {
      this->tour.push_back(*it);
    }
  }
}