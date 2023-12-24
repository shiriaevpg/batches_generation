//
// Created by vlad on 24.12.23.
//
#include "../utility/utility.cpp"
#include "../genetic_algorithm/gen_algorithm.h"
#include "../simulated_annealing/sim_annealing.h"
#include "../simulated_annealing/asd_mutator.h"
#include <iostream>
#include <chrono>

int main() {
  int size = 100;
  Parser parser("../../data/datasets/moscow_claims_2023-09-01-170.000000.csv");
  auto orders = parser.Get();
  auto traces = SeparateOnTrace(orders);
  SimAnnealing<Mutator, DefaultScheduler> sim_annealing(orders);
  std::vector<BatchT> ancestors;
  ancestors.reserve(size);
  for (int i = 0; i < size; ++i) {
    ancestors.push_back(sim_annealing.GenerateBatch(traces[0], 10000));
  }
  long double old_average_len = 0;
  for (int i = 0; i < size; ++i) {
    auto len = GetLength(ancestors[i], orders);
    old_average_len += len / ancestors[i].first.size();
  }
  old_average_len /= size;
  auto old_time = std::chrono::high_resolution_clock::now();
  auto descendants = Evolution(ancestors, 1000, 5, 0.1, orders);
  auto new_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(new_time - old_time);
  std::cout << "Time: " << time_span.count() << "s" << std::endl;
  long double new_average_len = 0;
  int new_size = descendants.size();
  for (int i = 0; i < new_size; ++i) {
    auto len = GetLength(descendants[i], orders);
    new_average_len += len / descendants[i].first.size();
  }
  new_average_len /= new_size;
  std::cout << "Old average length: " << old_average_len << std::endl;
  std::cout << "New average length: " << new_average_len << std::endl;
}
