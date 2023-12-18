#pragma once
#include "order.h"

#include <vector>
#include <fstream>
#include <unordered_map>
#include <cmath>

const char* kColumns = ",claim_uuid,destination_lat,destination_lon,source_location_lat,source_location_lon,trace_id,zone_id\n";
const char* kCity = "moscow";

std::string GetRow(size_t id, const Order& order) {
  return std::to_string(id) + ',' + order.claim + ',' + std::to_string(order.destination.latitude) + ',' +
         std::to_string(order.destination.longitude) + ',' + std::to_string(order.source.latitude) + ',' +
         std::to_string(order.source.longitude) + ',' + order.trace + ',' + kCity + '\n';
}

// для тестирования удобно смотреть, как быстро/хорошо работает на маленьких и больших trace
void MakeOtherWithThreshold(const std::vector<std::vector<size_t>>& traces, const std::vector<Order>& orders,
                            const std::string& orig_filename, size_t left_threshold, size_t right_threshold) {
  std::ofstream file;
  file.open(std::string(orig_filename.begin(), orig_filename.end() - 4) + '_' +
            std::to_string(left_threshold) + '_' + std::to_string(right_threshold) + ".csv");
  file << kColumns;
  size_t id = 0;
  for (auto& trace : traces) {
    if (trace.size() >= left_threshold and trace.size() <= right_threshold) {
      for (auto claim_id : trace) {
        file << GetRow(id, orders[claim_id]);
        ++id;
      }
    }
  }
}

// оставляет только n / scale записей, но с таким же распределением по размеру trace
void MakeOtherWithScale(const std::vector<std::vector<size_t>>& traces, const std::vector<Order>& orders,
                        const std::string& orig_filename, double scale) {
  std::ofstream file;
  file.open(std::string(orig_filename.begin(), orig_filename.end() - 4) + '-' + std::to_string(scale) + ".csv");
  file << kColumns;
  size_t id = 0;
  std::unordered_map<size_t, size_t> counts;
  for (const auto& trace : traces) {
    if (counts.find(trace.size()) == counts.end()) {
      counts[trace.size()] = 1;
    } else {
      ++counts[trace.size()];
    }
  }
  for (auto& [size, count] : counts) {
    count = std::ceil(static_cast<double>(count) / scale);
  }
  for (auto& trace : traces) {
    if (counts[trace.size()] != 0) {
      for (auto claim_id : trace) {
        file << GetRow(id, orders[claim_id]);
        ++id;
      }
      --counts[trace.size()];
    }
  }
}