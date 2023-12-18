#include "sim_annealing.h"
#include "asd_mutator.h"
#include "../utility/utility.cpp"

#include <iomanip>
#include <iostream>
#include <chrono>

int main() {
  std::cout << std::setprecision(20);
  std::string filename = "../../data/datasets/moscow_claims_2023-09-01-170.000000.csv";
  Parser parser(filename.c_str());
  const auto& orders = parser.Get();
  auto traces = SeparateOnTrace(orders);
  std::cout << "parsing " << filename << " done" << std::endl;
  SimAnnealing<Mutator, DefaultScheduler> sim_annealing(orders);
  std::cout << "With " << SimAnnealing<Mutator, DefaultScheduler>::kDefaultIterCount << "iter count\n";
  std::cout << "Приблизительное время работы " << SimAnnealing<Mutator, DefaultScheduler>::CountTime(traces) << " c" << std::endl;

  Gainer gainer(orders);
  BatchT batch;
  long double gain = 0;

  std::chrono::time_point<std::chrono::high_resolution_clock> start_point, end_point;
  start_point = std::chrono::high_resolution_clock::now(); // storing the starting time point in start

  for (const auto& trace : traces) {
    batch = sim_annealing.GenerateBatch(trace);
    gain += gainer.Get(batch);
  }

  end_point = std::chrono::high_resolution_clock::now(); //storing the ending time in end

  auto start = time_point_cast<std::chrono::microseconds>(start_point).time_since_epoch().count();
  // casting the time point to microseconds and measuring the time since time epoch

  auto end = time_point_cast<std::chrono::microseconds>(end_point).time_since_epoch().count();

  std::cout<<"Time taken = "<<(end-start)<<" microseconds"<<std::endl;
  std::cout << "summary gain: " << gain << " kilometers\n";
  std::cout << "average gain: " << gain / traces.size() << " kilometers \n";
}