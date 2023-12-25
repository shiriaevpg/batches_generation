#include "../utility/utility.cpp"

#include <string>
#include <iostream>
#include <iomanip>

// use google benchmark library: https://github.com/google/benchmark
#include <benchmark/benchmark.h>

std::vector<Order> orders;
std::vector<std::vector<size_t>> traces;
std::vector<size_t> fattest;

std::string kCurrentPath;
std::string kStatisticPath;
std::vector<size_t> times;

void GeneralMain(int argc, char** argv, const std::string& dataset) {
  std::cout << std::setprecision(20);
  Parser parser(dataset.c_str());
  orders = parser.Get();
  traces = SeparateOnTrace(orders);
  std::cout << "parsing " << dataset << " done" << std::endl;;
  const size_t kFattestSize = 500;
  for (const auto& trace : traces) {
    if (trace.size() == kFattestSize) {
      fattest = trace;
      break;
    }
  }
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
}

template <typename Algo>
void BM_fattest(benchmark::State& state, Algo& algo) {
  BatchT batch;
  for (auto _ : state) {
    benchmark::DoNotOptimize(batch = algo.GenerateBatch(fattest));
  }
}

size_t ToSizeT(const std::string& number) {
  auto d_number = std::stold(number);
  return static_cast<size_t>(d_number);
}

std::string GetCurrTime() {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%d-%m-%Y-%H-%M");
  auto str = oss.str();
  return str;
}

std::pair<std::string, std::string> GetNames(const std::string& name) {
  auto time = GetCurrTime();
  return {name + "-" + time + ".txt", name + "-" + time + ".png"};
}

std::vector<size_t> ParseBenchmark(const std::string& filepath) {
  std::vector<size_t> ans;
  static const size_t kFirstUnnecessaryLines = 10;
  static const size_t kTimeNumber = 2;
  static const size_t kColumnsCount = 10;
  static const char kDelimiter = ',';
  std::fstream fin;
  fin.open(filepath, std::ios::in);
  std::string line;
  for (size_t i = 0; i < kFirstUnnecessaryLines; ++i) {
    std::getline(fin, line);
  }
  std::vector<std::string> cells(kColumnsCount);
  std::string new_cell;
  while (fin >> line) {
    std::stringstream str(line);
    for (size_t j = 0; j < kColumnsCount; ++j) {
      getline(str, cells[j], kDelimiter);
    }
    ans.push_back(ToSizeT(cells[kTimeNumber]));
  }
  fin.close();
  return ans;
}

template <typename Algo>
void Pipeline(const Algo& algo, const std::string& name, size_t number) {
  auto [statistic, out] = GetNames(name);
  CollectStatistic(algo, kStatisticPath + statistic, times[number]);
  system((std::string("/usr/bin/python3 ../graphic.py") + " " + kStatisticPath + statistic + " " + kStatisticPath + out + " " + name).c_str());
}

template <typename Algo>
void CollectStatistic(const Algo& algo, const std::string& out_file, size_t time) {
  Gainer gainer(orders);
  std::vector<Statistic> stats;
  BatchT batch;
  for (const auto& trace : traces) {
    batch = algo.GenerateBatch(trace);
    auto size = batch.first.size();
    auto [old_len, len] = gainer.GetLengths(batch);
    stats.emplace_back(size, old_len, len);
  }
  std::ofstream out (out_file, std::ofstream::out);
  out << static_cast<double>(time) / 1e9 << '\n';
  out.precision(10);
  for (const auto& stat : stats) {
    out << stat.size << " " << stat.old_len << " " << stat.len << std::endl;
  }
}