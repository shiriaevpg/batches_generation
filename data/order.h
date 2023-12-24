#pragma once
#include <string>
#include <iostream>

struct Point {
  long double latitude = -1;
  long double longitude = -1;

  Point(const std::string& str_latitude, const std::string& str_longitude);
  constexpr Point(long double latitude, long double longitude);
  Point(const Point&) = default;
  auto operator<=>(const Point&) const = default;
  Point& operator+=(const Point& other);
  Point& operator-=(const Point& other);
  Point& operator/=(long double divider);
  Point& operator*=(long double multiplier);
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

constexpr Point::Point(long double latitude, long double longitude) : latitude(latitude), longitude(longitude) {}

Point& Point::operator+=(const Point& other) {
  longitude += other.longitude;
  latitude += other.latitude;
  return *this;
}

Point& Point::operator-=(const Point& other) {
  longitude -= other.longitude;
  latitude -= other.latitude;
  return *this;
}

Point& Point::operator/=(long double divider) {
  longitude /= divider;
  latitude /= divider;
  return *this;
}

Point& Point::operator*=(long double multiplier) {
  longitude *= multiplier;
  latitude *= multiplier;
  return *this;
}

Point operator+(Point point, const Point& other) {
  point += other;
  return point;
}

Point operator-(Point point, const Point& other) {
  point -= other;
  return point;
}

Point operator/(Point point, long double divider) {
  point /= divider;
  return point;
}

Point operator*(long double divider, Point point) {
  point *= divider;
  return point;
}

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