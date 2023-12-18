#include "../simulated_annealing/sim_annealing.h"
#include "../simulated_annealing/asd_mutator.h"
#include "../utility/utility.cpp"

#include <iomanip>
#include <iostream>

// use google benchmark library: https://github.com/google/benchmark
#include <benchmark/benchmark.h>

std::vector<Order> orders;
std::vector<std::vector<size_t>> traces;
std::vector<size_t> fattest;

static void BM_all(benchmark::State& state) {
  Gainer gainer(orders);
  SimAnnealing<Mutator, DefaultScheduler> sim_annealing(orders);
  BatchT batch;
  long double gain;
  for (auto _ : state) {
    gain = 0;
    for (const auto& trace : traces) {
      gain += gainer.Get(sim_annealing.GenerateBatch(trace, state.range(0)));
    }
    std::cout << "Average gain :" << gain / traces.size() << '\n';
  }
}
BENCHMARK(BM_all)->Arg(1000)->Arg(10000)->Arg(5000);

static void BM_fattest(benchmark::State& state) {
  Gainer gainer(orders);
  SimAnnealing<Mutator, DefaultScheduler> sim_annealing(orders);
  BatchT batch;
  for (auto _ : state) {
    std::cout << "Gain :" << gainer.Get(sim_annealing.GenerateBatch(fattest, state.range(0))) << '\n';
  }
}
//BENCHMARK(BM_fattest)->Arg(1000)->Arg(10000)->Arg(100000);

int main(int argc, char** argv) {
  std::cout << std::setprecision(20);
  std::string filename = "../../data/datasets/moscow_claims_2023-09-01-170.000000.csv";
  Parser parser(filename.c_str());
  orders = parser.Get();
  traces = SeparateOnTrace(orders);
  std::cout << "parsing " << filename << " done" << std::endl;
  const size_t kFattestSize = 500;
  for (const auto& trace : traces) {
    if (trace.size() == kFattestSize) {
      fattest = trace;
      break;
    }
  }
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
}