#include "sim_annealing.h"
#include "asd_mutator.h"

#include <iostream>

long double GetLength(const Point& source, const Point& destination) {  // заменяем ту фигню с широтой и долготой на Евклидову норму чтобы тестировать было легче
  if (source.latitude < 0 or destination.latitude < 0) {  // удобно, когда в порядке обхода в батче нет следующего и предыдущего, считать, что расстояние 0
    return 0;
  }
  return std::sqrt(std::pow(destination.latitude - source.latitude, 2) + std::pow(destination.longitude - source.longitude, 2));
}

int main() {
  std::vector<Order> orders = {{"6", "3", "1", "1", "1", "1"},
                               {"7", "3", "2", "1", "2", "1"},
                               {"7", "4", "2", "2", "3", "1"},
                               {"6", "4", "1", "2", "4", "1"}};
  auto traces = SeparateOnTrace(orders);
  SimAnnealing<Mutator, DefaultScheduler> sim_annealing(orders);
  Gainer gainer(orders);
  for (const auto& trace : traces) {
    std::cout << gainer.Get(sim_annealing.GenerateBatch(trace)) << std::endl;
  }
}