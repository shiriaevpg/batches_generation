#include "sim_annealing.h"
#include "asd_mutator_cm.h"
#include "../utility/utility_debug.cpp"

#include <iostream>

std::ostream& operator<<(std::ostream& out, const BatchT& batch) {
  for (auto id : batch.first) {
    std::cout << id << ' ';
  }
  for (auto id : batch.second) {
    std::cout << id << ' ';
  }
  std::cout << std::endl;
}

void EffectivityTest(const std::vector<size_t>& iter_counts, size_t size, const std::vector<Order>& trace) {
  auto orders = MakeOrders(std::vector<std::vector<Order>>(size, trace));
  auto traces = SeparateOnTrace(orders);
  SimAnnealing<MutatorCM, DefaultScheduler> sim_annealing(orders);
  Gainer gainer(orders);
  long double sum_gain;
  for (auto iter : iter_counts) {
    sum_gain = 0;
    for (const auto& tr : traces) {
      sum_gain += gainer.GetAbs(sim_annealing.GenerateBatch(tr, iter));
    }
    std::cout << "Average gain with iter_count " << iter << " is " << sum_gain / size << std::endl;
  }
}

int main() {
  auto tr1 = MakeTrace({
                           {{1, 1}, {6, 3}},
                           {{2, 1}, {7, 3}},
                           {{2, 2}, {7, 4}},
                           {{1, 2}, {6, 4}}
                           // макс значение, которое можно получить по нашей метрике по этому trace равно 17.41755 или 4sqrt(29) - (6 + sqrt(17)) + 3len_approach
  });
  EffectivityTest({10, 100, 300, 1000}, 20, tr1);
}