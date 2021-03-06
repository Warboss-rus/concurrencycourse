#include "JoinableThread.h"
#include "ThreadPool.h"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <functional>
#include <future>
#include <vector>

using Container = std::vector<int>;
using Iterator = Container::iterator;
using SorterFunc = std::function<void(Iterator begin, Iterator end)>;

void fill_container(Container& data, int size);

// Do not modify
void single_threaded_sort(Iterator begin, Iterator end)
{
	if (std::distance(begin, end) <= 1)
	{
		return;
	}

	auto divide_point = begin + std::distance(begin, end) / 2;
	std::nth_element(begin, divide_point, end);

	single_threaded_sort(begin, divide_point);
	single_threaded_sort(divide_point + 1, end);
}

// Optimize me
void multithreaded_sort(Iterator begin, Iterator end)
{
	if (std::distance(begin, end) <= 1)
	{
		return;
	}

	auto divide_point = begin + std::distance(begin, end) / 2;
	std::nth_element(begin, divide_point, end);

	JoinableThread<std::thread> asyncTask(multithreaded_sort, begin, divide_point);

	multithreaded_sort(divide_point + 1, end);
}

// Do not modify
void sort_impl(benchmark::State& state, const SorterFunc& sorterFunc)
{
	for (auto _ : state)
	{
		state.PauseTiming();
		Container data;
		fill_container(data, state.range());
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
void multithreaded(benchmark::State& state)
{
	sort_impl(state, multithreaded_sort);
}

// Do not modify
void fill_container(Container& data, int size)
{
	data.reserve(size);
	srand(0);
	for (int i = 0; i < size; ++i)
	{
		data.push_back(rand());
	}
}

constexpr int ITERATIONS = 5;
constexpr int RANGE_LIMIT = 8 << 8;

BENCHMARK(multithreaded)->Range(8, RANGE_LIMIT)->Threads(1)->Iterations(ITERATIONS);
BENCHMARK(single_threaded)->Range(8, RANGE_LIMIT)->Threads(1)->Iterations(ITERATIONS);