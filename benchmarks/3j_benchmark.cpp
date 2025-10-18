#include "benchmark/benchmark.h"
#include "wigcpp/wigcpp.hpp"

static void BM_3j(benchmark::State& state) {
  wigcpp::global_init(2 * 100, 9);
  for(auto _ : state){
    auto res = wigcpp::three_j(2 * 15, 2 * 30, 2 * 40, 2 * 2, 2 * 2, -2 * 4);
    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}


BENCHMARK(BM_3j);

BENCHMARK_MAIN();