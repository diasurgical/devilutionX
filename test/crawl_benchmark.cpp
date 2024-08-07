#include <benchmark/benchmark.h>

#include "crawl.hpp"
#include "engine/displacement.hpp"

namespace devilution {
namespace {

void BM_Crawl(benchmark::State &state)
{
	const int radius = static_cast<int>(state.range(0));
	for (auto _ : state) {
		int sum;
		Crawl(0, radius, [&sum](Displacement d) {
			sum += d.deltaX + d.deltaY;
			return false;
		});
		benchmark::DoNotOptimize(sum);
	}
}

BENCHMARK(BM_Crawl)->RangeMultiplier(4)->Range(1, 20);

} // namespace
} // namespace devilution
