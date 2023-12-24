//
// Created by vlad on 24.12.23.
//
#pragma once
#include "../utility/utility.h"
#include <random>

std::vector<size_t> GetIncreasingPerm(size_t old_size, size_t new_size) {
  std::vector<size_t> perm(old_size);
  std::iota(perm.begin(), perm.end(), 0);
  std::shuffle(perm.begin(), perm.end(), gen);
  perm.resize(new_size);
  std::sort(perm.begin(), perm.end());
  return perm;
}

std::vector<size_t> GetReverseLinks(const std::vector<size_t>& links) {
  std::vector<size_t> reverse_links(links.size());
  for (size_t i = 0; i < links.size(); ++i) {
    reverse_links[links[i]] = i;
  }
  return reverse_links;
}

std::vector<BatchT> GetChilds(BatchT mom, BatchT dad, size_t count) {
  size_t mom_size = mom.first.size();
  size_t dad_size = dad.first.size();
  size_t min_size = std::min(mom_size, dad_size);
  size_t max_size = std::max(mom_size, dad_size);
  std::vector<size_t> order(mom_size + dad_size);
  std::fill(order.begin(), order.begin() + mom_size, 1);
  std::vector<BatchT> childs(count);
  for (size_t i = 0; i < count; ++i) {
    std::shuffle(order.begin(), order.end(), gen);
    size_t child_size =
        std::uniform_int_distribution<size_t>(min_size, max_size)(gen);
    size_t mom_part =
        std::accumulate(order.begin(), order.begin() + child_size, 0);
    size_t dad_part = child_size - mom_part;
    std::vector<size_t> compressed_mom_src;
    compressed_mom_src.reserve(mom_part);
    std::vector<size_t> compressed_dad_src;
    compressed_dad_src.reserve(dad_part);
    auto mom_perm = GetIncreasingPerm(mom_size, mom_part);
    auto dad_perm = GetIncreasingPerm(dad_size, dad_part);
    std::set<size_t> mom_set;
    std::set<size_t> dad_set;
    for (size_t j = 0; j < mom_part; ++j) {
      mom_set.insert(mom.first[mom_perm[j]]);
    }
    for (size_t j = 0; j < dad_part; ++j) {
      dad_set.insert(dad.first[dad_perm[j]]);
    }
    for (auto& elem : mom.second) {
      if (mom_set.contains(elem)) {
        compressed_mom_src.push_back(elem);
      }
    }
    for (auto& elem : dad.second) {
      if (dad_set.contains(elem)) {
        compressed_dad_src.push_back(elem);
      }
    }
    size_t mom_ptr = 0;
    size_t dad_ptr = 0;
    childs[i].first.reserve(child_size);
    childs[i].second.reserve(child_size);
    for (size_t j = 0; j < child_size; ++j) {
      if (order[j] == 1) {
        childs[i].first.push_back(mom.first[mom_perm[mom_ptr]]);
        childs[i].second.push_back(compressed_mom_src[mom_ptr]);
        ++mom_ptr;
      } else {
        childs[i].first.push_back(dad.first[dad_perm[dad_ptr]]);
        childs[i].second.push_back(compressed_dad_src[dad_ptr]);
        ++dad_ptr;
      }
    }
  }
  return childs;
}

std::vector<BatchT> SingleGenEvolution(const std::vector<BatchT>& parents,
                                       size_t child_count,
                                       double mutation_prob,
                                       const std::vector<Order>& orders) {
  size_t parents_count = parents.size();
  std::vector<size_t> perm(parents_count);
  std::iota(perm.begin(), perm.end(), 0);
  std::shuffle(perm.begin(), perm.end(), gen);
  std::vector<BatchT> childs;
  childs.reserve(parents_count / 2 * child_count);
  for (size_t i = 0; i < parents_count; i += 2) {
    auto& mom = parents[perm[i]];
    auto& dad = parents[perm[i + 1]];
    auto childs_batch = GetChilds(mom, dad, child_count);
    childs.insert(childs.end(), childs_batch.begin(), childs_batch.end());
  }
  std::vector<std::pair<long double, int>> lengths(childs.size());
  for (size_t i = 0; i < childs.size(); ++i) {
    lengths[i].first = {GetLength(childs[i], orders) / childs[i].first.size()};
    lengths[i].second = i;
  }
  std::sort(lengths.begin(), lengths.end(), [](auto& a, auto& b) {
    return a.first < b.first;
  });
  size_t result_size = child_count * mutation_prob;
  std::vector<BatchT> result(result_size);
  for (size_t i = 0; i < result_size; ++i) {
    result[i] = childs[lengths[i].second];
  }
  return result;
}

std::vector<BatchT> Evolution(const std::vector<BatchT>& ancestors,
                              size_t gen_count,
                              size_t child_count,
                              double mutation_prob,
                              const std::vector<Order>& orders) {
  std::vector<BatchT> result = ancestors;
  for (size_t i = 0; i < gen_count; ++i) {
    result = SingleGenEvolution(result, 10, mutation_prob, orders);
    if (result.size() > ancestors.size()) {
      result.resize(ancestors.size());
    } else if (result.size() < ancestors.size()) {
      int delta = ancestors.size() - result.size();
      for (int i = 0; i < delta; ++i) {
        result.push_back(result[i]);
      }
    }
  }
  return result;
}


