cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

project(sim_annealing_test)

find_package(benchmark REQUIRED)

add_executable(sim_annealing_test sim_annealing/test.cpp)
target_link_libraries(sim_annealing_test benchmark::benchmark)

add_executable(sim_annealing_cm_test sim_annealing_cm/test.cpp)
target_link_libraries(sim_annealing_cm_test benchmark::benchmark)