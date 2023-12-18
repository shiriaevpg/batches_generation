// utility to help for testing/debugging

#include "utility.h"

long double GetLength(const Point& source, const Point& destination) {  // заменяем ту фигню с широтой и долготой на
                                                                        // Евклидову норму, чтобы тестировать/дебаггать было легче
  if (source.latitude < 0 or destination.latitude < 0) {  // удобно, когда в порядке обхода в батче нет следующего и предыдущего,
                                                          // считать, что расстояние 0
    return 0;
  }
  return std::sqrt(std::pow(destination.latitude - source.latitude, 2) + std::pow(destination.longitude - source.longitude, 2));
}

std::vector<Order> MakeTrace(const std::vector<std::pair<Point, Point>>& claims) {
  std::vector<Order> trace;
  trace.reserve(claims.size());
  for (const auto& claim : claims) {
    trace.emplace_back(claim.first, claim.second);
  }
  return trace;
}

std::vector<Order> MakeOrders(const std::vector<std::vector<Order>>& traces) {
  std::vector<Order> orders;
  for (size_t i = 0; i < traces.size(); ++i) {
    for (size_t j = 0; j < traces[i].size(); ++j) {
      orders.push_back(traces[i][j]);
      orders[orders.size() - 1].trace = std::to_string(i);
    }
  }
  return orders;
}