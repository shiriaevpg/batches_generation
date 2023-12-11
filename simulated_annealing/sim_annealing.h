#pragma once
#include <random>
#include "../utility.h"

struct DefaultScheduler {
  constexpr static long double kDefaultSchedulingStep = 0.99;
  long double scheduling_step = kDefaultSchedulingStep;
  long double operator()(long double temp) const {
    return temp * scheduling_step;
  }
};

// как мне показалось, легче всего сделать самую общую версию, а потом подменять параметры
template <typename Mutator, typename Scheduler = DefaultScheduler>
class SimAnnealing {
 private:
  const std::vector<Order>& orders_;
  Scheduler scheduler_;  // изменяет температуру

  static const size_t kDefaultIterCount = 1000;
  constexpr static long double kDefaultTemperature = 1;

 public:
  explicit SimAnnealing(const std::vector<Order>& orders);
  BatchT GenerateBatch(const std::vector<size_t>& trace, size_t iter_count = kDefaultIterCount,
                       long double temp = kDefaultTemperature);
};

template <typename Mutator, typename Scheduler>
SimAnnealing<Mutator, Scheduler>::SimAnnealing(const std::vector<Order>& orders) : orders_(orders) {}

template <typename Mutator, typename Scheduler>
std::pair<std::vector<size_t>, std::vector<size_t>>
SimAnnealing<Mutator, Scheduler>::GenerateBatch(
    const std::vector<size_t>& trace, size_t iter_count, long double temp) {
  if (trace.size() == 1) {
    return {trace, trace};
  }
  Mutator mutator(orders_, trace);
  std::uniform_int_distribution<size_t> distribution(0, trace.size() - 1);
  auto begin_claim = distribution(gen);  // закидываем рандомный заказ в батч
  mutator.Add(trace[begin_claim]);
  long double prob;
  decltype(mutator()) changes;  // сюда пишем изменения
  for (size_t i = 0; i < iter_count; ++i) {
    changes = mutator();
    prob = mutator.GetProb(changes, temp);
    std::discrete_distribution<> distr({static_cast<double>(1 - prob), static_cast<double>(prob)});
    // не знаю почему, но discrete_distribution принимает только даблы
    if (distr(gen)) {
      mutator.Change(changes);
    }
    temp = scheduler_(temp);
  }
  return mutator.GetBatch();
}