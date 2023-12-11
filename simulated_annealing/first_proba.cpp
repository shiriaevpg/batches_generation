#include "sim_annealing.h"
#include "asd_mutator.h"

#include <iostream>

int main() {
  Parser parser("../../data/moscow_claims_2023-09-01.csv", 100000);
  const auto& orders = parser.Get();
  auto traces = SeparateOnTrace(orders);
  SimAnnealing<Mutator, DefaultScheduler> sim_annealing(orders);
  Gainer gainer(orders);
  for (const auto& trace : traces) {
    std::cout << gainer.Get(sim_annealing.GenerateBatch(trace)) << std::endl;
  }
}