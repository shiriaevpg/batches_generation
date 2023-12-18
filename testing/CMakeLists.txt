cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

project(proba)

find_package(benchmark REQUIRED)

#add_executable(proba test.cpp)
add_executable(proba sim_annealing.cpp)
target_link_libraries(proba benchmark::benchmark)