// тут всякая шняга, которая может пригодится везде

#pragma once
#include "../data/parser.h"

#include <cmath>
#include <map>
#include <numbers>
#include <random>
#include <set>

using BatchT = std::pair<std::vector<size_t>, std::vector<size_t>>;

// используем метрику sum(dest - src) - sum(в порядке обхода в батче) + len_approach(n - 1),
// так как в каждом отдельном заказе еще есть подлет
const long double kLengthApproach = 2; // длина подлёта в километрах

std::random_device rd;
std::mt19937 gen(rd());

long double GetLength(const Point& source, const Point& destination); // возвращает длину маршрута в километрах

long double GetLength(const BatchT &batch, const std::vector<Order> &orders) {
  long double len = 0;
  for (size_t i = 1; i < batch.first.size(); ++i) {
    len += GetLength(orders[batch.first[i - 1]].destination, orders[batch.first[i]].source);
  }
  len += GetLength(orders[batch.first.back()].destination, orders[batch.second[0]].destination);
  for (size_t i = 1; i < batch.first.size(); ++i) {
    len += GetLength(orders[batch.first[i - 1]].destination, orders[batch.first[i]].destination);
  }
  return len;
}

template <typename SetT, typename Func>  // рандомный элемент из set/map
size_t GetRandomElement(const SetT& set, Func&& func) {  // func нужен, чтобы избежать копипасты, в случае когда ищу по set и map
  auto min_element = *set.begin();
  auto max_element = *(--set.end());
  std::uniform_int_distribution<size_t> distribution(func(min_element), func(max_element));
  auto random_element = distribution(gen);
  auto lb = set.lower_bound(random_element);
  return func(*lb);
}

template <typename SetT, typename Func>
std::pair<size_t, size_t> GetRandomElements(
    const SetT& set, Func&& func) { // 2 рандомных различных элемента из set/map
  auto random_element1 = GetRandomElement(set, std::forward<Func>(func));
  auto random_element2 = GetRandomElement(set, std::forward<Func>(func));
  while (random_element1 == random_element2) {
    random_element2 = GetRandomElement(set, std::forward<Func>(func));
  }
  return {random_element1, random_element2};
}

// считает выгоду по метрике
class Gainer {
 private:
  const std::vector<Order>& orders_;

 public:
  explicit Gainer(const std::vector<Order>& orders) : orders_(orders) {}
  [[nodiscard]] long double Get(const BatchT& batch) const;
};

long double Gainer::Get(const BatchT& batch) const {
  long double default_len = GetLength(orders_[batch.first[0]].source, orders_[batch.first[0]].destination);
  long double batch_len = GetLength(batch, orders_);
  return default_len - batch_len + kLengthApproach * (batch.second.size() - 1);
}
