#pragma once
#include "order.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>

class Parser {
 private:
  static const size_t kDefaultCountRows = 17586688;  // wc -l data/moscow_claims_2023-09-01.csv
  static const size_t kClaimIdNumber = 1;
  static const size_t kDestLatNumber = 2;
  static const size_t kDestLonNumber = 3;
  static const size_t kSrcLatNumber = 4;
  static const size_t kSrcLonNumber = 5;
  static const size_t kTraceIdNumber = 6;
  static const size_t kColumnsCount = 7;
  static const char kDelimiter = ',';

  std::vector<Order> orders_;

 public:
  explicit Parser(const char* filepath, size_t count_rows = kDefaultCountRows);
  [[nodiscard]] const std::vector<Order>& Get() const;
};

const std::vector<Order>& Parser::Get() const { return orders_; }

Parser::Parser(const char* filepath, size_t count_rows) {
  orders_.reserve(count_rows);
  std::fstream fin;
  fin.open(filepath, std::ios::in);
  std::string line;
  std::getline(fin, line);
  std::vector<std::string> cells(kColumnsCount);
  while (fin >> line) {
    std::stringstream str(line);
    for (size_t j = 0; j < kColumnsCount; ++j) {
      getline(str, cells[j], kDelimiter);
    }
    orders_.emplace_back(cells[kDestLatNumber], cells[kDestLonNumber],
                         cells[kSrcLatNumber], cells[kSrcLonNumber],
                         std::move(cells[kClaimIdNumber]),
                         std::move(cells[kTraceIdNumber]));
  }
  fin.close();
}

std::vector<std::vector<size_t>> SeparateOnTrace(
    const std::vector<Order>& orders) {
  std::unordered_map<std::string, std::vector<size_t>> traces;
  for (size_t i = 0; i < orders.size(); ++i) {
    traces[orders[i].trace].push_back(i);
  }
  std::vector<std::vector<size_t>> ans;
  ans.reserve(traces.size());
  for (auto& trace : traces) {
    ans.push_back(std::move(trace.second));
  }
  return ans;
}