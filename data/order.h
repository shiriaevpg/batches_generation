#pragma once
#include <string>
#include <iostream>

struct Point {
  long double latitude = -1;
  long double longitude = -1;

  Point(const std::string& str_latitude, const std::string& str_longitude);
  Point(long double latitude, long double longitude);
  Point(const Point&) = default;
  auto operator<=>(const Point&) const = default;
  Point() = default;
};

struct Order {
  Point destination;
  Point source;
  std::string claim;
  std::string trace;

  Order(const std::string& dest_lat, const std::string& dest_lon,
        const std::string& src_lat, const std::string& src_lon,
        std::string&& claim, std::string&& trace);
  Order(const Point& source, const Point& destination);
  Order(const Order&) = default;
};

Point::Point(const std::string& str_latitude, const std::string& str_longitude)
    : latitude(std::stold(str_latitude)),
      longitude(std::stold(str_longitude)) {}

Point::Point(long double latitude, long double longitude) : latitude(latitude), longitude(longitude) {}

Order::Order(const std::string& dest_lat, const std::string& dest_lon,
             const std::string& src_lat, const std::string& src_lon,
             std::string&& claim, std::string&& trace)
    : destination(dest_lat, dest_lon),
      source(src_lat, src_lon),
      claim(std::move(claim)),
      trace(std::move(trace)) {}

Order::Order(const Point& source, const Point& destination)
    : destination(destination), source(source) {}

const Point kDefaultPoint = Point();

std::ostream& operator<<(std::ostream& out, const Order& order) {
  out << "src: " << order.source.latitude << ' ' << order.source.longitude << '\n';
  out << "dest: " << order.destination.latitude << ' ' << order.destination.longitude << '\n';
  return out;
}