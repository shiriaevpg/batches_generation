#include "../pipeline.h"
#include "../../simulated_annealing/sim_annealing.h"
#include "../../simulated_annealing/asd_mutator_cm.h"

using TestT = SimAnnealing<MutatorCM, DefaultScheduler>;
TestT sim_annealing1(orders, 10000, 0.2, 0.2, 0.3, 0.3, 1, 10);
TestT sim_annealing2(orders, 10000, 0.2, 0.2, 0.3, 0.3, 1, 5);
TestT sim_annealing3(orders, 10000, 0.05, 0.05, 0.45, 0.45, 1, 10);
TestT sim_annealing4(orders, 10000, 0.05, 0.05, 0.45, 0.45, 1, 5);
TestT sim_annealing5(orders, 10000, 0.1, 0.1, 0.4, 0.4, 1, 10);
TestT sim_annealing6(orders, 10000, 0.1, 0.1, 0.4, 0.4, 1, 5);

BENCHMARK_CAPTURE(BM_fattest, 1, sim_annealing1);
BENCHMARK_CAPTURE(BM_fattest, 2, sim_annealing2);
BENCHMARK_CAPTURE(BM_fattest, 3, sim_annealing3);
BENCHMARK_CAPTURE(BM_fattest, 4, sim_annealing4);
BENCHMARK_CAPTURE(BM_fattest, 5, sim_annealing5);
BENCHMARK_CAPTURE(BM_fattest, 6, sim_annealing6);

int main(int argc, char** argv) {
  kCurrentPath = "../sim_annealing_cm/";
  kStatisticPath = kCurrentPath + "statistic/";
  GeneralMain(argc, argv, "../../data/datasets/moscow_claims_2023-09-01-170.000000.csv");
  times = ParseBenchmark(kCurrentPath + "statistic/benchmark.csv");
  Pipeline(sim_annealing1, "Отжигцм_кол-во_итераций=10000,вероятности=0.2,0.2,0.3,0.3,рад=10", 0);
  Pipeline(sim_annealing2, "Отжигцм_кол-во_итераций=10000,вероятности=0.2,0.2,0.3,0.3,рад=5", 1);
  Pipeline(sim_annealing3, "Отжигцм_кол-во_итераций=10000,вероятности=0.05,0.05,0.45,0.45,рад=10", 2);
  Pipeline(sim_annealing4, "Отжигцм_кол-во_итераций=10000,вероятности=0.05,0.05,0.45,0.45,рад=5", 3);
  Pipeline(sim_annealing5, "Отжигцм_кол-во_итераций=10000,вероятности=0.1,0.1,0.4,0.4,рад=10", 4);
  Pipeline(sim_annealing6, "Отжигцм_кол-во_итераций=10000,вероятности=0.1,0.1,0.4,0.4,рад=5", 5);
}