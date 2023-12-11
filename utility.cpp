#include "utility.h"

long double GetLength(const Point& source, const Point& destination) {
  if (source.latitude < 0 or destination.latitude < 0) {  // удобно, когда в порядке обхода в батче нет следующего и предыдущего, считать, что расстояние 0
    return 0;
  }
  static const long long kEarthRadius = 6371;
  static const long double kConvCoefficient = std::numbers::pi / 180;
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