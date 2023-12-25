#include "../pipeline.h"
#include "../../simulated_annealing/sim_annealing.h"
#include "../../simulated_annealing/asd_mutator.h"

using TestT = SimAnnealing<Mutator, DefaultScheduler>;
TestT sim_annealing1(orders, 1000);

BENCHMARK_CAPTURE(BM_fattest, default_sim_annealing, sim_annealing1);

int main(int argc, char** argv) {
  kCurrentPath = "../sim_annealing/";
  kStatisticPath = kCurrentPath + "statistic/";
  GeneralMain(argc, argv, "../../data/datasets/moscow_claims_2023-09-01-170.000000.csv");
  times = ParseBenchmark(kCurrentPath + "statistic/benchmark.csv");
  Pipeline(sim_annealing1, "default", 0);
}