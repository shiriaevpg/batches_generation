//
// Created by ruslan on 26.11.23.
//
#include <random>
#include <vector>
#include "../data/parsing/parser.h"

// TODO как мне показалось, легче всего сделать самую общую версию, а потом подменять параметры
template <typename Mutation, typename Scheduler, typename CalcProb, typename Change>
class SimAnnealing {
 private:
  Mutation mutation_;
  Scheduler scheduler_;
  CalcProb calc_prob_;
  Change changer_;

  static const size_t kDefaultIterCount = 1000;
  constexpr static long double kDefaultTemperature = 100;

 public:
  std::pair<std::vector<size_t>, std::vector<size_t>>
  GenerateBatch(const std::vector<size_t>& trace,
                size_t iter_count = kDefaultIterCount,
                long double temp = kDefaultTemperature);
};

template <typename Mutation, typename Scheduler, typename CalcProb, typename Change>
std::pair<std::vector<size_t>, std::vector<size_t>>
SimAnnealing<Mutation, Scheduler, CalcProb, Change>::GenerateBatch(
    const std::vector<size_t>& trace, size_t iter_count, long double temp) {
  std::vector<size_t> sources;
  std::vector<size_t> destinations;
  sources.reserve(trace.size());  // мб не надо, но кажется так будет быстрее
  destinations.reserve(trace.size());
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> distribution(0, trace.size() - 1);
  auto claim = distribution(gen);  // закидываем рандомный заказ в батч
  sources.push_back(claim);
  destinations.push_back(claim);
  long double prob;
  decltype(mutation_(sources, destinations, temp)) changes;
  for (size_t i = 0; i < iter_count; ++i) {
    changes = mutation_(sources, destinations, temp);
    prob = calc_prob_(sources, destinations, changes);
    std::discrete_distribution<> distr({static_cast<double>(1 - prob), static_cast<double>(prob)});  // TODO не знаю почему, но discrete_distribution принимает только даблы
    if (distr(gen)) {
      changer_(sources, destinations, changes);
    }
    temp = scheduler_(temp);
  }
  return {sources, destinations};
}

// TODO
class Generator {
 private:
  constexpr static long double kDefaultSchedulingStep = 0.99;


 public:
  std::vector<size_t> GenerateBatch(const std::vector<size_t>& trace);
};



int main() {
  Parser parser("data/moscow_claims_2023-09-01.csv");
  auto traces = SeparateOnTrace(parser.Get());
  std::cout << "traces count: " << traces.size() << '\n';
  size_t max = 0;
  for (auto& trace : traces) {
    max = std::max(trace.size(), max);
  }
  std::cout << max;
}