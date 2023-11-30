#include <random>
#include <vector>
#include <cmath>
#include <map>
#include <set>
#include "../data/parsing/parser.h"

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
  std::vector<Order>* orders_;
  Scheduler scheduler_;  // изменяет температуру

  static const size_t kDefaultIterCount = 1000;
  constexpr static long double kDefaultTemperature = 100;

 public:
  std::pair<std::vector<size_t>, std::vector<size_t>>
  GenerateBatch(const std::vector<size_t>& trace,
                size_t iter_count = kDefaultIterCount,
                long double temp = kDefaultTemperature);
};

template <typename Mutator, typename Scheduler>
std::pair<std::vector<size_t>, std::vector<size_t>>
SimAnnealing<Mutator, Scheduler>::GenerateBatch(
    const std::vector<size_t>& trace, size_t iter_count, long double temp) {
  std::set<size_t> sources;
  std::set<size_t> destinations;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> distribution(0, trace.size() - 1);
  auto claim = distribution(gen);  // закидываем рандомный заказ в батч
  sources.insert(0);
  destinations.insert(0);
  Mutator mutator(orders_, trace, claim);
  long double prob;
  decltype(mutator(sources, destinations)) changes;  // сюда пишем изменения
  for (size_t i = 0; i < iter_count; ++i) {
    changes = mutator(sources, destinations);
    prob = mutator.GetProb(sources, destinations, changes, temp);
    std::discrete_distribution<> distr({static_cast<double>(1 - prob), static_cast<double>(prob)});
                                       // не знаю почему, но discrete_distribution принимает только даблы
    if (distr(gen)) {
      mutator.Change(sources, destinations, changes);
    }
    temp = scheduler_(temp);
  }
  return mutator.ToVectors(sources, destinations);
}

class Generator {
 private:
  class Mutator {
   private:
    std::vector<Order>* orders_;
    std::set<size_t> used_;
    std::set<size_t> not_used_;
    std::map<size_t, size_t> src_pos_;  // return pos in sources
    std::map<size_t, size_t> src_id_;  // return id in sources
    std::map<size_t, size_t> dest_pos_;
    std::map<size_t, size_t> dest_id_;

    // TODO тут хз какие вероятности, можно сделать так, чтобы когда в батче мало заказов вероятность добавить была больше и наоборот
    constexpr static double kRemoveProb = 0.1;
    constexpr static double kAddProb = 0.1;
    constexpr static double kSwapSrcProb = 0.4;
    constexpr static double kSwapDestProb = 0.4;

    [[nodiscard]] double GetRemoveProb() const { return kRemoveProb; }
    [[nodiscard]] double GetAddProb() const { return kAddProb; }
    [[nodiscard]] double GetSwapSrcProb() const { return kSwapSrcProb; }
    [[nodiscard]] double GetSwapDestProb() const { return kSwapDestProb; }
    void Remove(std::set<size_t>& sources, std::set<size_t>& destinations, size_t id);
    void Add(std::set<size_t>& sources, std::set<size_t>& destinations, size_t id);
    void Swap(std::map<size_t, size_t>& id_to_pos, std::map<size_t, size_t>& pos_to_id, size_t id1, size_t id2);
    static size_t GetRandomElement(const std::set<size_t>& set);
    static std::pair<size_t, size_t> GetRandomElements(const std::set<size_t>& set);

   public:
    enum class MutationType : size_t {
      kRemove,
      kAdd,
      kSwapSrc,
      kSwapDest
    };
    struct Mutation {
      MutationType mutation_type;
      size_t number1;  // pos заказов, которые нужно swapнуть или pos заказа, которые нужно удалить / добавить
      size_t number2;

      Mutation(MutationType mutation_type, const std::pair<size_t, size_t>& swap_poses);
      Mutation(MutationType mutation_type, size_t pos);
    };
    Mutator(const std::vector<size_t>& trace, size_t claim);
    Mutation operator()(const std::set<size_t>& sources, const std::set<size_t>& destinations) const;
    void Change(const Mutation& mutation, std::set<size_t>& sources, std::set<size_t>& destinations);
    std::pair<std::vector<size_t>, std::vector<size_t>> ToVectors(
        const std::set<size_t>& sources, const std::set<size_t>& destinations);
    long double GetProb(const std::set<size_t>& sources, const std::set<size_t>& destinations, const Mutation& mutation, long double temp);
  };

 public:
  std::vector<size_t> GenerateBatch(const std::vector<size_t>& trace); // TODO
};

Generator::Mutator::Mutation::Mutation(Generator::Mutator::MutationType mutation_type, size_t pos)
    : mutation_type(mutation_type), number1(pos) {}

Generator::Mutator::Mutation::Mutation(Generator::Mutator::MutationType mutation_type,
                                       const std::pair<size_t, size_t>& swap_poses)
    : mutation_type(mutation_type), number1(swap_poses.first), number2(swap_poses.second) {}

std::pair<std::vector<size_t>, std::vector<size_t>> Generator::Mutator::ToVectors(
    const std::set<size_t>& sources, const std::set<size_t>& destinations) {
  std::vector<size_t> sources_ans;
  sources_ans.reserve(sources.size());
  std::vector<size_t> destinations_ans;
  destinations_ans.reserve(destinations.size());
  for (auto pos : sources) {
    sources_ans.push_back(src_id_[pos]);
  }
  for (auto pos : destinations) {
    destinations_ans.push_back(dest_id_[pos]);
  }
  return {sources_ans, destinations_ans};
}

Generator::Mutator::Mutator(const std::vector<size_t>& trace, size_t claim)
    : not_used_(trace.begin(), trace.end()) {
  used_.insert(claim);
  not_used_.erase(claim);
  src_pos_[claim] = 0;
  src_id_[0] = claim;
  dest_pos_[claim] = 0;
  dest_id_[0] = claim;
}

void Generator::Mutator::Remove(std::set<size_t>& sources, std::set<size_t>& destinations, size_t id) {
  used_.erase(id);
  not_used_.insert(id);
  sources.erase(src_pos_[id]);
  src_id_.erase(src_pos_[id]);
  src_pos_.erase(id);
  destinations.erase(dest_pos_[id]);
  dest_id_.erase(dest_pos_[id]);
  dest_pos_.erase(id);
}

void Generator::Mutator::Add(std::set<size_t>& sources, std::set<size_t>& destinations, size_t id) {
  used_.insert(id);
  not_used_.erase(id);
  auto new_pos_src = *(--sources.end()) + 1; // вставлю всегда в конец
                                                         // (можно поменять и вставлять в рандомное место)
  sources.insert(new_pos_src);
  src_pos_[id] = new_pos_src;
  src_id_[new_pos_src] = id;
  auto new_pos_dest = *(--destinations.end()) + 1;
  destinations.insert(new_pos_dest);
  dest_pos_[id] = new_pos_dest;
  dest_id_[new_pos_dest] = id;
}

void Generator::Mutator::Swap(
    std::map<size_t, size_t>& id_to_pos, std::map<size_t, size_t>& pos_to_id,
    size_t id1, size_t id2) {
  std::swap(id_to_pos[id1], id_to_pos[id2]);
  pos_to_id[id_to_pos[id1]] = id1;
  pos_to_id[id_to_pos[id2]] = id2;
}

void Generator::Mutator::Change(
    const Generator::Mutator::Mutation& mutation, std::set<size_t>& sources,
    std::set<size_t>& destinations) {
  switch (mutation.mutation_type) {
    case MutationType::kRemove:
      Remove(sources, destinations, mutation.number1);
    case MutationType::kAdd:
      Add(sources, destinations, mutation.number1);
    case MutationType::kSwapSrc:
      Swap(src_pos_, src_id_, mutation.number1, mutation.number2);
    case MutationType::kSwapDest:
      Swap(dest_pos_, dest_id_, mutation.number1, mutation.number2);
  }
}

size_t Generator::Mutator::GetRandomElement(const std::set<size_t>& set) {
  auto min_element = *set.begin();
  auto max_element = *(--set.end());
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> distribution(min_element, max_element);
  auto random_element = distribution(gen);
  auto lb = set.lower_bound(random_element);
  return *lb;
}

Generator::Mutator::Mutation Generator::Mutator::operator()(
    const std::set<size_t>& sources, const std::set<size_t>& destinations) const {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::discrete_distribution<size_t> distr({GetRemoveProb(), GetAddProb(), GetSwapSrcProb(), GetSwapDestProb()});
  MutationType mutation_type{distr(gen)};
  if (mutation_type == MutationType::kRemove) {
    return {mutation_type, GetRandomElement(used_)};
  }
  if (mutation_type == MutationType::kAdd) {
    return {mutation_type, GetRandomElement(not_used_)};
  }
  if (mutation_type == MutationType::kSwapSrc) {
    return {mutation_type, GetRandomElements(sources)};
  }
  return {mutation_type, GetRandomElements(destinations)};
}

std::pair<size_t, size_t> Generator::Mutator::GetRandomElements(const std::set<size_t>& set) {
  auto random_element1 = GetRandomElement(set);
  auto random_element2 = GetRandomElement(set);
  while (random_element1 == random_element2) {
    random_element2 = GetRandomElement(set);
  }
  return {random_element1, random_element2};
}

long double Generator::Mutator::GetProb(const std::set<size_t>& sources,
                                        const std::set<size_t>& destinations,
                                        const Generator::Mutator::Mutation& mutation,
                                        long double temp) {
  long double diff;
  switch (mutation.mutation_type) {
    case MutationType::kRemove:
      // TODO тут надо посчиатать насколько изменилось состояние по нашей метрике
  }
  return std::exp(diff / temp);
}

int main() {
//  Parser parser("data/moscow_claims_2023-09-01.csv");
//  auto traces = SeparateOnTrace(parser.Get());
//  std::cout << "traces count: " << traces.size() << '\n';
//  size_t max = 0;
//  for (auto& trace : traces) {
//    max = std::max(trace.size(), max);
//  }
//  std::cout << max;
}