#pragma once
#include "../utility/utility.h"

#include <random>

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
  size_t iter_count_;
  long double begin_temp_;

  long double remove_prob_;
  long double add_prob_;
  long double swap_src_prob_;
  long double swap_dest_prob_;
  long double cm_threshold_;

  constexpr static long double kDefaultTemperature = 1;
  constexpr static double kRemoveProb = 0.1;
  constexpr static double kAddProb = 0.1;
  constexpr static double kSwapSrcProb = 0.4;
  constexpr static double kSwapDestProb = 0.4;

  constexpr static double kCMThreshold = 40;  // если расстояние от cm больше threshold, то не добавляем точку

 public:
  static const size_t kDefaultIterCount = 10000;
  // считает примерное время работы:
  static long double CountTime(const std::vector<std::vector<size_t>>& traces, size_t iter_count=kDefaultIterCount);
  explicit SimAnnealing(const std::vector<Order>& orders, size_t iter_count = kDefaultIterCount,
                        long double remove_prob = kRemoveProb, long double add_prob = kAddProb,
                        long double swap_src_prob = kSwapSrcProb, long double swap_dest_prob = kSwapDestProb,
                        long double begin_temp = kDefaultTemperature, long double cm_threshold = kCMThreshold);
  [[nodiscard]] BatchT GenerateBatch(const std::vector<size_t>& trace) const;
};

template <typename Mutator, typename Scheduler>
SimAnnealing<Mutator, Scheduler>::SimAnnealing(
    const std::vector<Order>& orders, size_t iter_count, long double remove_prob, long double add_prob,
    long double swap_src_prob, long double swap_dest_prob, long double begin_temp, long double cm_threshold)
    : orders_(orders), iter_count_(iter_count), begin_temp_(begin_temp),
      remove_prob_(remove_prob), add_prob_(add_prob), swap_src_prob_(swap_src_prob), swap_dest_prob_(swap_dest_prob),
      cm_threshold_(cm_threshold) {}

template <typename Mutator, typename Scheduler>
std::pair<std::vector<size_t>, std::vector<size_t>>
SimAnnealing<Mutator, Scheduler>::GenerateBatch(const std::vector<size_t>& trace) const {
  if (trace.size() == 1) {
    return {trace, trace};
  }
  auto temp = begin_temp_;
  Mutator mutator(orders_, trace, remove_prob_, add_prob_, swap_src_prob_, swap_dest_prob_, cm_threshold_);
  std::uniform_int_distribution<size_t> distribution(0, trace.size() - 1);
  auto begin_claim = distribution(gen);  // закидываем рандомный заказ в батч
  mutator.Add(trace[begin_claim]);
  long double prob;
  decltype(mutator(temp)) changes;  // сюда пишем изменения
  for (size_t i = 0; i < iter_count_; ++i) {
    changes = mutator(temp);
    prob = mutator.GetProb(changes, temp);
    std::discrete_distribution<> distr({static_cast<double>(1 - prob), static_cast<double>(prob)});
    // не знаю почему, но discrete_distribution принимает только даблы
    if (distr(gen)) {
      mutator.Change(changes);
    }
    temp = scheduler_(temp);
  }
  auto batch = mutator.GetBatch();
  return batch;
}

template <typename Mutator, typename Scheduler>
long double SimAnnealing<Mutator, Scheduler>::CountTime(const std::vector<std::vector<size_t>>& traces, size_t iter_count) {
  std::unordered_map<size_t, size_t> counts;
  for (const auto& trace : traces) {
    if (counts.find(trace.size()) == counts.end()) {
      counts[trace.size()] = 1;
    } else {
      ++counts[trace.size()];
    }
  }
  long double ans = 0;
  for (const auto& [size, count] : counts) {
    ans += static_cast<long double>(count) * std::log2(size);
  }
  return 1.2 * ans * iter_count / 10e5;
}
