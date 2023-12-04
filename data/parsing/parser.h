#include <cmath>
#include <fstream>
#include <ios>
#include <iostream>
#include <numbers>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

const long long kEarthRadius = 6371;
const long double kConvCoefficient = std::numbers::pi / 180;

struct Point {
  long double latitude;
  long double longitude;

  Point(const std::string& str_latitude, const std::string& str_longitude);
};

class Order {
 public:
  Point destination;
  Point source;
  std::string claim;
  std::string trace;

  Order(const std::string& dest_lat, const std::string& dest_lon,
        const std::string& src_lat, const std::string& src_lon,
        std::string&& claim, std::string&& trace);
  long double GetLength() const;
};

class Parser {
 private:
  static const size_t kDefaultCountRows = 17586688;
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

long double Order::GetLength() const {
  static const long double kOneLatitude = 111.11;

  //формула отсюда:
  // https://stackoverflow.com/questions/27928/calculate-distance-between-two-latitude-longitude-points-haversine-formula
  long double latDelta = destination.latitude - source.latitude;
  long double lonDelta = destination.longitude - source.longitude;
  latDelta *= kConvCoefficient;
  lonDelta *= kConvCoefficient;
  long double angle = std::pow(std::sin(latDelta / 2), 2) +
                      std::pow(std::sin(lonDelta / 2), 2) +
                      std::cos(source.latitude * kConvCoefficient) *
                          std::cos(destination.latitude * kConvCoefficient);
  return angle * kEarthRadius;
}

const std::vector<Order>& Parser::Get() const { return orders_; }

Point::Point(const std::string& str_latitude, const std::string& str_longitude)
    : latitude(std::stold(str_latitude)),
      longitude(std::stold(str_longitude)) {}

Order::Order(const std::string& dest_lat, const std::string& dest_lon,
             const std::string& src_lat, const std::string& src_lon,
             std::string&& claim, std::string&& trace)
    : destination(dest_lat, dest_lon),
      source(src_lat, src_lon),
      claim(std::move(claim)),
      trace(std::move(trace)) {}

Parser::Parser(const char* filepath, size_t count_rows) {
  orders_.reserve(count_rows);
  std::fstream fin;
  fin.open(filepath, std::ios::in);
  std::string line;
  std::getline(fin, line);
  std::vector<std::string> cells(kColumnsCount);
  for (size_t i = 0; i < count_rows; ++i) {
    fin >> line;
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