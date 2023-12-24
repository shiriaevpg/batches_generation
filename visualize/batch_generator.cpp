//
// Created by vlad on 20.12.23.
//
#include "../simulated_annealing/sim_annealing.h"
#include "../simulated_annealing/asd_mutator.h"
#include "../utility/utility.cpp"

#include <fstream>

int main(int argc, char** argv) {
  int size = std::atoi(argv[1]);
  std::cout << size << std::endl;
  auto path = "batch.txt";
  std::ofstream out(path, std::ios::trunc);
  if (!out.is_open()) {
    std::cout << "Can't open file" << std::endl;
    return 1;
  }
  out.precision(20);
  out << size << std::endl;
  Parser parser("../data/datasets/moscow_claims_2023-09-01-170.000000.csv");
  auto orders = parser.Get();
  auto traces = SeparateOnTrace(orders);
  SimAnnealing<Mutator, DefaultScheduler> sim_annealing(orders);
  int max_len = 10;
  for (int i = 0; i < size; ++i) {
    BatchT batch = sim_annealing.GenerateBatch(traces[0], 10000);
    while (batch.first.size() > max_len) {
      batch = sim_annealing.GenerateBatch(traces[0], 10000);
    }
    out << batch.first.size() * 2 << std::endl;
    for (auto id : batch.first) {
      out << orders[id].source.latitude << ' '
          << orders[id].source.longitude << '\n';
    }
    for (auto id : batch.second) {
      out << orders[id].destination.latitude << ' '
          << orders[id].destination.longitude << '\n';
    }
  }
  return 0;
}
