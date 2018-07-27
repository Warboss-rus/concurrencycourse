#include "ThreadPool.h"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <functional>
#include <future>
#include <vector>

using Container = std::vector<int>;
using Iterator = Container::iterator;
using SorterFunc = std::function<void(Iterator begin, Iterator end)>;

// Do not modify
void single_threaded_sort(Iterator begin, Iterator end)
{
	if (begin == end)
	{
		return;
	}

	auto partition_val = *begin;
	auto divide_point = std::stable_partition(begin, end, [=](auto const& val) { return val < partition_val; });

	single_threaded_sort(begin, divide_point);
	single_threaded_sort(++divide_point, end);
}

// Do not modify
void unoptimized_sort(Iterator begin, Iterator end)
{
	if (begin == end)
	{
		return;
	}

	auto partition_val = *begin;
	auto divide_point = std::stable_partition(begin, end, [=](auto const& val) { return val < partition_val; });

	std::thread asyncTask(unoptimized_sort, begin, divide_point);

	unoptimized_sort(++divide_point, end);

	asyncTask.join();
}

// Optimize me
void optimized_sort(Iterator begin, Iterator end)
{
	if (begin == end)
	{
		return;
	}

	auto partition_val = *begin;
	auto divide_point = std::stable_partition(begin, end, [=](auto const& val) { return val < partition_val; });

	std::thread asyncTask(optimized_sort, begin, divide_point);

	optimized_sort(++divide_point, end);

	asyncTask.join();
}

// Do not modify
void sort_impl(benchmark::State& state, const SorterFunc& sorterFunc)
{
	for (auto _ : state)
	{
		state.PauseTiming();
		Container data;
		srand(0);
		for (int i = 0; i < state.range(); ++i)
		{
			data.push_back(rand());
		}
		state.ResumeTiming();
		sorterFunc(data.begin(), data.end());
		state.PauseTiming();
		if (!std::is_sorted(data.begin(), data.end()))
		{
			throw std::logic_error("the range is not sorted correctly");
		}
		state.ResumeTiming();
	}
}

// Do not modify
void single_threaded(benchmark::State& state)
{
	sort_impl(state, single_threaded_sort);
}

// Do not modify
void unoptimized(benchmark::State& state)
{
	sort_impl(state, unoptimized_sort);
}

// Do not modify
void optimized(benchmark::State& state)
{
	sort_impl(state, optimized_sort);
}

constexpr int ITERATIONS = 10;
constexpr int RANGE_LIMIT = 8 << 8;

BENCHMARK(unoptimized)->Range(8, RANGE_LIMIT)->Threads(1)->Iterations(ITERATIONS);
BENCHMARK(optimized)->Range(8, RANGE_LIMIT)->Threads(1)->Iterations(ITERATIONS);
BENCHMARK(single_threaded)->Range(8, RANGE_LIMIT)->Threads(1)->Iterations(ITERATIONS);