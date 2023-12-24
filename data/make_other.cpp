#include "make_other.h"
#include "parser.h"

#include <iostream>

int main() {
  std::string filename = "datasets/moscow_claims_2023-09-01.csv";
  Parser parser(filename.c_str(), true);
  const auto& orders = parser.Get();
  auto traces = SeparateOnTrace(orders);
  std::cout << "End parsing" << std::endl;
  MakeOtherWithThreshold(traces, orders, filename, 1, 30);
  std::cout << "Make with thresholds 1 and 30 end" << std::endl;
  MakeOtherWithThreshold(traces, orders, filename, 31, 100);
  std::cout << "Make with thresholds 31 and 100 end" << std::endl;
  MakeOtherWithThreshold(traces, orders, filename, 101, 1000);
  std::cout << "Make with thresholds 101 and 1000 end" << std::endl;
  MakeOtherWithScale(traces, orders, filename, 17); // примерно миллион записей должно получиться
  std::cout << "Make with scale 17 end" << std::endl;
  MakeOtherWithScale(traces, orders, filename, 170); // примерно 100к записей
  std::cout << "Make with scale 170 end" << std::endl;
}