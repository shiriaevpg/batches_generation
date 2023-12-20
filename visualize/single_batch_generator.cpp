//
// Created by vlad on 20.12.23.
//
#include "../simulated_annealing/sim_annealing.h"
#include "../simulated_annealing/asd_mutator.h"
#include "../utility/utility.cpp"

#include <fstream>

int main(int argc, char** argv) {
  auto path = argv[1];
  std::ofstream out(path);
  if (!out.is_open()) {
    std::cout << "Can't open file" << std::endl;
    return 1;
  }
  out.clear();
  out.precision(20);
  Parser parser("../../data/datasets/moscow_claims_2023-09-01-170.000000.csv");
  auto orders = parser.Get();
  auto traces = SeparateOnTrace(orders);
  SimAnnealing<Mutator, DefaultScheduler> sim_annealing(orders);
  BatchT batch;
  for (int i = 0; i < 10; ++i) {
    auto lbatch = sim_annealing.GenerateBatch(traces[0], 1000);
    auto len = lbatch.first.size();
    if (len > batch.first.size() && len < 10) {
      batch = lbatch;
      break;
    }
  }
  std::cout << batch.first.size() << std::endl;
  for (auto id : batch.first) {
    out << orders[id].destination.latitude << ' '
        << orders[id].destination.longitude << '\n';
  }
  return 0;
}