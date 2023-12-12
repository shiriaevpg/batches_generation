#include "sim_annealing.h"
#include "asd_mutator.h"
#include "../utility/utility.cpp"

#include <iostream>

int main() {
//  Parser parser("../../data/moscow_claims_2023-09-01.csv");
//  const auto& orders = parser.Get();
//  auto traces = SeparateOnTrace(orders);
//  std::cout << "parsing done" << std::endl;
//  SimAnnealing<Mutator, DefaultScheduler> sim_annealing(orders);
//  Gainer gainer(orders);
//  BatchT batch;
//  for (const auto& trace : traces) {
//    if (trace.size() > 4) {
//      std::cout << "trace size is " << trace.size() << '\n';
//      batch = sim_annealing.GenerateBatch(trace);
//      std::cout << "batch size is " << batch.first.size() << '\n';
//      std::cout << "gain in kilometres is " << gainer.Get(batch) << std::endl;
//    }
//  }
  // TODO должно быть 755
  std::cout << GetLength({55.75059, 37.61777}, {50.44952, 30.52537});
}