#include <thread>
#include <future>
#include <benchmark/benchmark.h>

constexpr int NUMBER_OF_THREADS = 4;

int find_the_answer_to_ltuae()
{
	// Do not modify
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	return 42;
}

void unoptimized(benchmark::State& state)
{
	for (auto _ : state)
	{
		// Do not modify
		for (int i = 0; i < NUMBER_OF_THREADS; ++i)
		{
			std::async(std::launch::async, find_the_answer_to_ltuae);
		}
	}
}

void optimized(benchmark::State& state)
{
	for (auto _ : state)
	{
		// FIXME: please optimize here
		for (int i = 0; i < NUMBER_OF_THREADS; ++i)
		{
			std::async(std::launch::async, find_the_answer_to_ltuae);
		}
	}
}

BENCHMARK(unoptimized)->Iterations(20);
BENCHMARK(optimized)->Iterations(20);