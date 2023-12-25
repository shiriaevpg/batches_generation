#include "../simulated_annealing/sim_annealing.h"
#include "../simulated_annealing/asd_mutator.h"
#include "../simulated_annealing/asd_mutator_cm.h"
#include "../utility/utility.cpp"
#include <iostream>
#include <fstream>

std::vector<Order> orders;
std::vector<std::vector<size_t>> traces;
std::vector<size_t> fattest;

template<typename Algo = SimAnnealing<Mutator, DefaultScheduler>>
static std::vector<Statistic> GetStatistic(int iter_count) {
  Gainer gainer(orders);
  Algo algo(orders);
  std::vector<Statistic> stats;
  BatchT batch;
  for (const auto& trace : traces) {
    batch = algo.GenerateBatch(trace, iter_count);
    auto size = batch.first.size();
    auto [old_len, len] = gainer.GetLengths(batch);
    stats.emplace_back(size, old_len, len);
  }
  return stats;
}

Statistic GetAverage(const std::vector<Statistic>& stats) {
  Statistic result{};
  result.size = std::accumulate(stats.begin(), stats.end(), 0, [](size_t sum, const Statistic& stat) { return sum + stat.size; });
  result.old_len = std::accumulate(stats.begin(), stats.end(), 0, [](long double sum, const Statistic& stat) { return sum + stat.old_len; });
  result.len = std::accumulate(stats.begin(), stats.end(), 0, [](long double sum, const Statistic& stat) { return sum + stat.len; });
  return result;
}

int main(int argc, char** argv) {
  std::cout.precision(20);
  std::string filename = "../data/datasets/moscow_claims_2023-09-01-170.000000.csv";
  Parser parser(filename.c_str());
  orders = parser.Get();
  traces = SeparateOnTrace(orders);
  std::cout << "parsing " << filename << " done" << std::endl;;
  int iter_count =  std::atoi(argv[1]);
  if (!iter_count) {
    iter_count = 100;
  }

  std::cout << "Примерное время тестирования: " << SimAnnealing<MutatorCM, DefaultScheduler>::CountTime(traces, iter_count) / 60 << " минут" << std::endl;
  auto stats = GetStatistic<SimAnnealing<MutatorCM, DefaultScheduler>>(iter_count);
  std::cout << "Тестирование завершено" << std::endl;

  std::cout << stats.size() << std::endl;
  Statistic average = GetAverage(stats);
  std::cout << "Average size : " << average.size << '\n';
  std::cout << "Average gain : " << average.GetAbsGain() << " kilometers or " << average.GetPercentGain() << " % " << '\n';
  std::cout << "Old average len : " << average.GetOldAverageLen() << " kilometers" << '\n';
  std::cout << "Average len : " << average.GetAverageLen() << " kilometers" << '\n';

  std::ofstream out ("statistics.txt", std::ofstream::out);
  out.precision(10);
  std::cout << ":)" << std::endl;
  for (const auto& stat : stats) {
    out << stat.size << " " << stat.old_len << " " << stat.len << std::endl;
  }
}