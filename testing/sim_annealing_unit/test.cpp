#include "../pipeline.h"
#include "../../simulated_annealing/sim_annealing.h"
#include "../../simulated_annealing/asd_mutator_unit_cm.h"

using TestT = SimAnnealing<MutatorCMUnit, DefaultScheduler>;
//TestT sim_annealing1(orders, 1000);
//TestT sim_annealing2(orders, 10000);
//TestT sim_annealing3(orders, 10000, 0.2, 0.2, 0.3, 0.3);
TestT sim_annealing4(orders, 100000, 0.05, 0.05, 0.45, 0.45, 20);
TestT sim_annealing5(orders, 100000, 0.1, 0.1, 0.4, 0.4, 20);

//BENCHMARK_CAPTURE(BM_fattest, 1, sim_annealing1);
//BENCHMARK_CAPTURE(BM_fattest, 2, sim_annealing2);
//BENCHMARK_CAPTURE(BM_fattest, 3, sim_annealing3);
BENCHMARK_CAPTURE(BM_fattest, 4, sim_annealing4);
BENCHMARK_CAPTURE(BM_fattest, 5, sim_annealing5);

int main(int argc, char** argv) {
  kCurrentPath = "../sim_annealing_unit/";
  kStatisticPath = kCurrentPath + "statistic/";
  GeneralMain(argc, argv, "../../data/datasets/moscow_claims_2023-09-01-170.000000.csv");
  times = ParseBenchmark(kCurrentPath + "statistic/benchmark.csv");
//  Pipeline(sim_annealing1, "Отжиг_удел_кол-во_итераций=1000,вероятности=0.1,0.1,0.4,0.4", 0);
//  Pipeline(sim_annealing2, "Отжиг_удел_кол-во_итераций=10000,вероятности=0.1,0.1,0.4,0.4", 1);
//  Pipeline(sim_annealing3, "Отжиг_удел_кол-во_итераций=10000,вероятности=0.2,0.2,0.3,0.3", 2);
//  Pipeline(sim_annealing4, "Отжиг_удел_кол-во_итераций=10000,вероятности=0.05,0.05,0.45,0.45", 3);
  Pipeline(sim_annealing5, "Отжиг_удел_кол-во_итераций=100000,вероятности=0.1,0.1,0.4,0.4,темпарутра=20", 0);
  Pipeline(sim_annealing4, "Отжиг_удел_кол-во_итераций=100000,вероятности=0.05,0.05,0.45,0.45,температура=20", 1);
}