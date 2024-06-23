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

  models::Place* Solution::pickPlace(int * t, std::vector<models::Place *>* places, int overrideDuration) {

    models::Place * foundPlace = nullptr;

    for (auto it = places->begin(); it != places->end(); ++it) {

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

    for (auto it = this->tour.begin(); it != this->tour.end(); ++it) {
      set.insert(*it);
    }

    return set.size() < this->tour.size();
  }

  void Solution::removeDuplicates() {
    std::unordered_set<models::Place *> set;
    for (auto it = this->tour.begin(); it != this->tour.end(); ) {
      auto element = *it;
      if (set.find(*it) != set.end()) {
        set.insert(element);
        it = this->tour.erase(it);
      } else {
        set.insert(element);
        ++it;
      } 
    }
  }

  void Solution::repair(SolutionCompliant * compliant, std::default_random_engine* randomEngine) {
    this->removeDuplicates();

    std::vector<models::Place *> tourVector;
    for (auto it = this->tour.begin(); it != this->tour.end(); ++it) {
      tourVector.push_back(*it);
    }

    std::vector<models::Place *> solutionAttractions = Problem::findPlacesByType(&tourVector, models::PlaceType::attraction);
    std::vector<models::Place *> solutionRestaurants = Problem::findPlacesByType(&tourVector, models::PlaceType::restaurant);
    std::vector<models::Place *> solutionBars = Problem::findPlacesByType(&tourVector, models::PlaceType::bar);
    std::vector<models::Place *> solutionCafes = Problem::findPlacesByType(&tourVector, models::PlaceType::bar);

    std::set<models::Place *> solutionAttractionsSet(solutionAttractions.begin(), solutionAttractions.end());
    std::set<models::Place *> solutionRestaurantsSet(solutionRestaurants.begin(), solutionRestaurants.end());
    std::set<models::Place *> solutionBarsSet(solutionBars.begin(), solutionBars.end());
    std::set<models::Place *> solutionCafesSet(solutionCafes.begin(), solutionCafes.end());

    std::vector<models::Place *> remainingAttractions = problem->findPlacesByType(models::PlaceType::attraction, solutionAttractionsSet);
    std::vector<models::Place *> remainingRestaurants = problem->findPlacesByType(models::PlaceType::restaurant, solutionRestaurantsSet);
    std::vector<models::Place *> remainingBars = problem->findPlacesByType(models::PlaceType::bar, solutionBarsSet);
    std::vector<models::Place *> remainingCafes = problem->findPlacesByType(models::PlaceType::cafe, solutionCafesSet);

    std::shuffle(remainingAttractions.begin(), remainingAttractions.end(), *randomEngine);
    std::shuffle(remainingRestaurants.begin(), remainingRestaurants.end(), *randomEngine);
    std::shuffle(remainingBars.begin(), remainingBars.end(), *randomEngine);
    std::shuffle(remainingCafes.begin(), remainingCafes.end(), *randomEngine);

    bool breakfastFixed = false;
    bool lunchFixed = false;
    bool dinnerFixed = false;
    bool barFixed = false;

    bool breakfastDone = false;
    bool lunchDone = false;
    bool dinnerDone = false;
    bool barDone = false;

    models::Place * breakfast = nullptr;
    models::Place * lunch = nullptr;
    models::Place * dinner = nullptr;
    models::Place * bar = nullptr;

    this->tour.clear();

    const int t_start = problem->constraints->startTime;
    int t = t_start;

    int it_counter = 0;

    while (t < t_start + problem->constraints->totalDuration) {
      it_counter++;

      // Find Breakfast
      if (problem->constraints->breakfast &&
          problem->constraints->breakfastStart <= t && // t <= problem->constraints->breakfastEnd &&
          breakfast == nullptr &&
          (solutionCafes.size() + remainingCafes.size() > 0)) {

        // PRINTLN("Breakfast");
        
        if (solutionCafes.size()) breakfast = this->pickPlace(&t, &solutionCafes, problem->constraints->breakfastDuration);
        if (breakfast == nullptr) breakfast = this->pickPlace(&t, &remainingCafes, problem->constraints->breakfastDuration);

        if (breakfast != nullptr) continue;
      }

      // Find Lunch
      if (problem->constraints->lunch &&
          problem->constraints->lunchStart <= t && // t <= problem->constraints->lunchEnd &&
          lunch == nullptr &&
          (solutionRestaurants.size() + remainingRestaurants.size() > 0)) {

        // PRINTLN("Lunch");

        if (solutionRestaurants.size()) lunch = this->pickPlace(&t, &solutionRestaurants, problem->constraints->lunchDuration);
        if (lunch == nullptr) lunch = this->pickPlace(&t, &remainingRestaurants, problem->constraints->lunchDuration);

        if (lunch != nullptr) continue;
      }

      // Find Dinner
      if (problem->constraints->dinner &&
          problem->constraints->dinnerStart <= t && // t <= problem->constraints->dinnerEnd &&
          dinner == nullptr &&
          (solutionRestaurants.size() + remainingRestaurants.size() > 0)) {

        // PRINTLN("Dinner");

        if (solutionRestaurants.size()) dinner = this->pickPlace(&t, &solutionRestaurants, problem->constraints->dinnerDuration);
        if (dinner == nullptr) dinner = this->pickPlace(&t, &remainingRestaurants, problem->constraints->dinnerDuration);
 
        if (dinner != nullptr) continue;
      }

      // Find Bar
      if (problem->constraints->bar &&
          problem->constraints->barStart <= t &&
          bar == nullptr &&
          (solutionBars.size() + remainingBars.size() > 0)) {

        // PRINTLN("Bar");

        if (solutionBars.size()) bar = this->pickPlace(&t, &solutionBars, problem->constraints->barDuration);
        if (bar == nullptr) bar = this->pickPlace(&t, &remainingBars, problem->constraints->barDuration);

        if (bar != nullptr) continue;
      }

      // Stop looking for new places when we are at the bar = end of tour
      if (problem->constraints->bar && bar != nullptr) break;

      // Find attraction
      if (solutionAttractions.size() + remainingAttractions.size() > 0) {

        models::Place * foundAttraction = nullptr;
        if (solutionAttractions.size()) foundAttraction = this->pickPlace(&t, &solutionAttractions);
        if (foundAttraction == nullptr) foundAttraction = this->pickPlace(&t, &remainingAttractions);

        if (foundAttraction != nullptr) continue;
      }
      
      // By default advance by 10 minutes
      t += 10;
      // PRINTLN("Default");
    }
  }
}