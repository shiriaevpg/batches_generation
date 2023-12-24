cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

project(sim_annealing_test)

find_package(benchmark REQUIRED)

add_executable(sim_annealing_test sim_annealing.cpp)
add_executable(gen_algorithm_test gen_algo_test.cpp)
target_link_libraries(sim_annealing_test benchmark::benchmark)