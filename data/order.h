#pragma once
#include <string>

struct Point {
  long double latitude = -1;
  long double longitude = -1;

  Point(const std::string& str_latitude, const std::string& str_longitude);
  Point() = default;
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
};

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

const Point kDefaultPoint = Point();