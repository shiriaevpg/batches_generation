// modification of asd MutatorCM
// по факту нужно наследование написать, но пока пофиг, копипаста

#pragma once
#include "../utility/utility.h"

#include <set>
#include <map>

class MutatorCM {
 public:
  enum class MutationType : size_t {
    kRemove,
    kAdd,
    kSwapSrc,
    kSwapDest,
    kNothing
  };
  struct Mutation {
    MutationType mutation_type;
    size_t number1{};
    size_t number2{};

    Mutation(MutationType mutation_type, const std::pair<size_t, size_t>& swap_poses);  // в случае, если swap
    Mutation(MutationType mutation_type, size_t pos);  // в случае, если добавлять или удалять
    Mutation() = default;
  };

  MutatorCM(const std::vector<Order>& orders, const std::vector<size_t>& trace);
  MutatorCM(const MutatorCM&) = default;
  Mutation operator()() const;
  void Add(size_t id);
  void Change(const Mutation& mutation);
  BatchT GetBatch();
  long double GetProb(const Mutation& mutation, long double temp);

 private:
  const std::vector<Order>& orders_;
  std::set<size_t> used_;
  std::set<size_t> not_used_;
  // поля, чтобы поддерживать связь pos и id
  // в src_id и dest_id по ключу хранится порядок, в котором надо обойти точки
  std::map<size_t, size_t> src_pos_;  // return pos in sources по id
  std::map<size_t, size_t> src_id_;  // return id in sources по pos
  std::map<size_t, size_t> dest_pos_;  // аналогично
  std::map<size_t, size_t> dest_id_;
  Point center_mass_ = {0, 0};

//  Gainer gainer_;  // для стресс тестов

  //  TODO тут хз какие вероятности, можно сделать так, чтобы когда в батче мало заказов,
  //  вероятность добавить была больше и наоборот
  constexpr static double kRemoveProb = 0.1;
  constexpr static double kAddProb = 0.1;
  constexpr static double kSwapSrcProb = 0.4;
  constexpr static double kSwapDestProb = 0.4;

  constexpr static double kCMThreshold = 10;  // если расстояние от cm больше threshold, то не добавляем точку
  static const size_t kTryingAddCount = 10; // выбираем kTryingAddCount раз точку, которую надо добавить, если не получилось, то мутации не будет

  [[nodiscard]] double GetRemoveProb() const { return used_.size() > 1 ? kRemoveProb : 0; }
  [[nodiscard]] double GetAddProb() const { return not_used_.empty() ? 0 : kAddProb; }
  [[nodiscard]] double GetSwapSrcProb() const { return used_.size() > 1 ? kSwapSrcProb : 0; }
  [[nodiscard]] double GetSwapDestProb() const { return used_.size() > 1 ? kSwapDestProb : 0; }

  void Remove(size_t id);
  static void Swap(std::map<size_t, size_t>& id_to_pos, std::map<size_t, size_t>& pos_to_id, size_t pos1, size_t pos2);

  [[nodiscard]] const Point& GetNextSrc(std::map<size_t, size_t>::iterator it) const;
  // возвращает следующую точку в порядке обхода в батче (может вернуть первую точку из dest)
  [[nodiscard]] const Point& GetNextDest(std::map<size_t, size_t>::iterator it) const;
  // аналогично, но если it == end - 1, то вернет Point()
  [[nodiscard]] const Point& GetPrevDest(std::map<size_t, size_t>::iterator it) const;
  // возвращает предыдущую точку в порядке обхода в батче (может вернуть последнюю точку из src)
  [[nodiscard]] const Point& GetPrevSrc(std::map<size_t, size_t>::iterator it) const;
  // аналогично, но если it == begin, то вернет Point()

  long double GetDiff(const Mutation& mutation);
  long double GetRemoveDiff(size_t id);
  long double GetAddDiff(size_t id);
  long double GetSwapSrcDiff(size_t pos1, size_t pos2);
  long double GetSwapDestDiff(size_t pos1, size_t pos2);

  static long double GetSwapDiff(const Point& prev1, const Point& point1, const Point& next1,
                                 const Point& prev2, const Point& point2, const Point& next2);
  static long double GetSwapNearDiff(const Point& prev, const Point& point1, const Point& point2, const Point& next);
};

MutatorCM::Mutation::Mutation(MutatorCM::MutationType mutation_type, size_t pos)
    : mutation_type(mutation_type), number1(pos) {}

MutatorCM::Mutation::Mutation(MutatorCM::MutationType mutation_type,
                            const std::pair<size_t, size_t>& swap_poses)
    : mutation_type(mutation_type), number1(swap_poses.first), number2(swap_poses.second) {}

BatchT MutatorCM::GetBatch() {
  std::vector<size_t> sources_ans;
  sources_ans.reserve(src_id_.size());
  std::vector<size_t> destinations_ans;
  destinations_ans.reserve(dest_id_.size());
  for (auto& [pos, id] : src_id_) {
    sources_ans.push_back(id);
  }
  for (auto& [pos, id] : dest_id_) {
    destinations_ans.push_back(id);
  }
  return {sources_ans, destinations_ans};
}

MutatorCM::MutatorCM(const std::vector<Order>& orders, const std::vector<size_t>& trace)
    : orders_(orders), not_used_(trace.begin(), trace.end()) {}

void MutatorCM::Remove(size_t id) {
  used_.erase(id);
  not_used_.insert(id);
  src_id_.erase(src_pos_[id]);
  src_pos_.erase(id);
  dest_id_.erase(dest_pos_[id]);
  dest_pos_.erase(id);
  center_mass_ += (2 * center_mass_ - orders_[id].source - orders_[id].destination) / (2 * used_.size());
}

void MutatorCM::Add(size_t id) {
  size_t new_pos_src = 0;
  size_t new_pos_dest = 0;
  if (!used_.empty()) {
    new_pos_src = (--src_id_.end())->first + 1; // вставлю всегда в конец
    // (можно поменять и вставлять в рандомное место)
    new_pos_dest = (--dest_id_.end())->first + 1;
  }
  used_.insert(id);
  not_used_.erase(id);
  src_pos_[id] = new_pos_src;
  src_id_[new_pos_src] = id;
  dest_pos_[id] = new_pos_dest;
  dest_id_[new_pos_dest] = id;
  center_mass_ += (orders_[id].source + orders_[id].destination - 2 * center_mass_) / (2 * used_.size());
}

void MutatorCM::Swap(
    std::map<size_t, size_t>& id_to_pos, std::map<size_t, size_t>& pos_to_id,
    size_t pos1, size_t pos2) {
  std::swap(pos_to_id[pos1], pos_to_id[pos2]);
  id_to_pos[pos_to_id[pos1]] = pos1;
  id_to_pos[pos_to_id[pos2]] = pos2;
}

void MutatorCM::Change(const MutatorCM::Mutation& mutation) {
  switch (mutation.mutation_type) {
    case MutationType::kRemove:
      Remove(mutation.number1);
      break;
    case MutationType::kAdd:
      Add(mutation.number1);
      break;
    case MutationType::kSwapSrc:
      Swap(src_pos_, src_id_, mutation.number1, mutation.number2);
      break;
    case MutationType::kSwapDest:
      Swap(dest_pos_, dest_id_, mutation.number1, mutation.number2);
    case MutationType::kNothing:break;
  }
}

MutatorCM::Mutation MutatorCM::operator()() const {
  std::discrete_distribution<size_t> distr({GetRemoveProb(), GetAddProb(), GetSwapSrcProb(), GetSwapDestProb()});
  MutationType mutation_type{distr(gen)};
  if (mutation_type == MutationType::kRemove) {
    return {mutation_type, GetRandomElement(used_, [](size_t a) { return a; })};
  }
  if (mutation_type == MutationType::kAdd) {
    auto new_claim = GetRandomElement(not_used_, [](size_t a) { return a; });
    size_t iter_count = 1;
    while (iter_count < kTryingAddCount and GetLength(orders_[new_claim].source, center_mass_) +
           GetLength(orders_[new_claim].destination, center_mass_) > 2 * kCMThreshold) {
      ++iter_count;
      new_claim = GetRandomElement(not_used_, [](size_t a) { return a; });
    }
    if (iter_count == kTryingAddCount) {
      return {MutationType::kNothing, 0};
    }
    return {mutation_type, new_claim};
  }
  if (mutation_type == MutationType::kSwapSrc) {
    return {mutation_type, GetRandomElements(src_id_, [](const std::map<size_t, size_t>::value_type& p) { return p.first;})};
  }
  return {mutation_type, GetRandomElements(dest_id_, [](const std::map<size_t, size_t>::value_type& p) { return p.first;})};
}

const Point& MutatorCM::GetNextSrc(std::map<size_t, size_t>::iterator it) const {
  if ((++it) == src_id_.end()) {
    return orders_[dest_id_.begin()->second].destination;
  }
  return orders_[it->second].source;
}

const Point& MutatorCM::GetNextDest(std::map<size_t, size_t>::iterator it) const {
  if ((++it) == dest_id_.end()) {
    return kDefaultPoint;
  }
  return orders_[it->second].destination;
}

const Point& MutatorCM::GetPrevSrc(std::map<size_t, size_t>::iterator it) const {
  if (it == src_id_.begin()) {
    return kDefaultPoint;
  }
  return orders_[(--it)->second].source;
}

const Point& MutatorCM::GetPrevDest(std::map<size_t, size_t>::iterator it) const {
  if (it == dest_id_.begin()) {
    return orders_[(--src_id_.end())->second].source;
  }
  return orders_[(--it)->second].destination;
}

long double MutatorCM::GetDiff(const MutatorCM::Mutation& mutation) {
  if (mutation.mutation_type == MutationType::kRemove) {
    return GetRemoveDiff(mutation.number1);
  }
  if (mutation.mutation_type == MutationType::kAdd) {
    return GetAddDiff(mutation.number1);
  }
  if (mutation.mutation_type == MutationType::kSwapSrc) {
    return GetSwapSrcDiff(mutation.number1, mutation.number2);
  }
  if (mutation.mutation_type == MutationType::kSwapDest) {
    return GetSwapDestDiff(mutation.number1, mutation.number2);
  }
  return 0;
}

long double MutatorCM::GetRemoveDiff(size_t id) {
  auto it_src = src_id_.find(src_pos_[id]);
  auto it_dest = dest_id_.find(dest_pos_[id]);
  const Point& src = orders_[id].source;
  const Point& dest = orders_[id].destination;
  auto prev_src = GetPrevSrc(it_src);
  auto next_src = GetNextSrc(it_src);
  auto prev_dest = GetPrevDest(it_dest);
  auto next_dest = GetNextDest(it_dest);
  if (it_dest == dest_id_.begin() and it_src == (--src_id_.end())) {  // если рядом, работает другая формула
    return GetLength(prev_src, src) + GetLength(dest, next_dest) -
        (GetLength(prev_src, next_dest) + kLengthApproach);
  }
  return GetLength(prev_src, src) + GetLength(src, next_src) +
      GetLength(prev_dest, dest) + GetLength(dest, next_dest) -
      (GetLength(src, dest) + GetLength(prev_src, next_src) +
          GetLength(prev_dest, next_dest) + kLengthApproach);
}

long double MutatorCM::GetAddDiff(size_t id) {
  const Point& prev_src = orders_[(--src_id_.end())->second].source;
  const Point& next_src = orders_[dest_id_.begin()->second].destination;
  const Point& prev_dest = orders_[(--dest_id_.end())->second].destination;
  const Point& src = orders_[id].source;
  const Point& dest = orders_[id].destination;
  return kLengthApproach + GetLength(src, dest) + GetLength(prev_src, next_src) -
      (GetLength(prev_src, src) + GetLength(src, next_src) + GetLength(prev_dest, dest));
}

long double MutatorCM::GetSwapSrcDiff(size_t pos1, size_t pos2) {
  auto it1 = src_id_.find(pos1);
  auto it2 = src_id_.find(pos2);
  const Point& src1 = orders_[it1->second].source;
  const Point& src2 = orders_[it2->second].source;
  auto prev1 = GetPrevSrc(it1);
  auto prev2 = GetPrevSrc(it2);
  auto next1 = GetNextSrc(it1);
  auto next2 = GetNextSrc(it2);
  if (++it1 == it2) {  // если рядом, работает другая формула
    return GetSwapNearDiff(prev1, src1, src2, next2);
  }
  if (++it2 == --it1) {
    return GetSwapNearDiff(prev2, src2, src1, next1);
  }
  return GetSwapDiff(prev1, src1, next1, prev2, src2, next2);
}

long double MutatorCM::GetSwapDestDiff(size_t pos1, size_t pos2) {
  auto it1 = dest_id_.find(pos1);
  auto it2 = dest_id_.find(pos2);
  const Point& dest1 = orders_[it1->second].destination;
  const Point& dest2 = orders_[it2->second].destination;
  auto prev1 = GetPrevDest(it1);
  auto prev2 = GetPrevDest(it2);
  auto next1 = GetNextDest(it1);
  auto next2 = GetNextDest(it2);
  if (++it1 == it2) {
    return GetSwapNearDiff(prev1, dest1, dest2, next2);
  }
  if (++it2 == --it1) {
    return GetSwapNearDiff(prev2, dest2, dest1, next1);
  }
  return GetSwapDiff(prev1, dest1, next1, prev2, dest2, next2);
}

long double MutatorCM::GetSwapDiff(
    const Point& prev1, const Point& point1, const Point& next1,
    const Point& prev2, const Point& point2, const Point& next2) {
  return GetLength(prev1, point1) + GetLength(point1, next1) +
      GetLength(prev2, point2) + GetLength(point2, next2) -
      (GetLength(prev1, point2) + GetLength(point2, next1) +
          GetLength(prev2, point1) + GetLength(point1, next2));
}

long double MutatorCM::GetSwapNearDiff(const Point& prev, const Point& point1, const Point& point2, const Point& next) {
  return GetLength(prev, point1) + GetLength(point2, next) -
      (GetLength(prev, point2) + GetLength(point1, next));
}

long double MutatorCM::GetProb(const MutatorCM::Mutation& mutation, long double temp) {
  return std::min(std::exp(GetDiff(mutation) / temp), static_cast<long double>(1));
}