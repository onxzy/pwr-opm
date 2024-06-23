cmake_minimum_required(VERSION 3.11)
project(opm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(Boost_DEBUG "Set to ON to enable debug output from FindBoost." OFF)
option(Boost_DETAILED_FAILURE_MSG "Set to ON to get detailed information" OFF)

set(BOOST_ROOT C:/Users/onxzy/dev/cpp-include/boost_1_82_0)

set(BOOST_INCLUDEDIR ${BOOST_ROOT}/boost)
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)

set(Boost_NO_BOOST_CMAKE TRUE)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost 1.82 COMPONENTS program_options REQUIRED)

if(NOT DEFINED Boost_FOUND)
  message(FATAL_ERROR "Boost not found")
endif()

include_directories(
  ${Boost_INCLUDE_DIRS}
)
link_directories(
  ${Boost_LIBRARY_DIRS}
)

add_library(
  utils STATIC
  src/utils/Config.cpp
  src/utils/Stats.cpp
)

add_library(
  models STATIC
  src/models/Map.cpp
  src/models/Place.cpp
)

add_library(
  problem STATIC
  src/problem/Problem.cpp
  src/problem/Solution.cpp
  src/problem/SolutionFitness.cpp
)

add_library(
  optimizer STATIC
  src/optimizer/Optimizer.cpp
  src/optimizer/FitnessMap.cpp
)

add_library(
  optimizer.random STATIC
  src/optimizer/Random.cpp
)

add_library(
  optimizer.ga STATIC
  src/optimizer/GA.cpp
  src/optimizer/ga/Crossover.cpp
  src/optimizer/ga/Mutation.cpp
  src/optimizer/ga/Selection.cpp
)

add_executable(Generate src/Generate.cpp)
target_link_libraries(Generate PRIVATE utils models problem)
target_link_libraries(Generate LINK_PUBLIC ${Boost_LIBRARIES})

add_executable(Main src/Main.cpp)
target_link_libraries(Main PRIVATE utils models problem optimizer optimizer.ga optimizer.random)
target_link_libraries(Main LINK_PUBLIC ${Boost_LIBRARIES})
