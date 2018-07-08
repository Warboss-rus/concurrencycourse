#include <thread>
#include <future>
#include "../benchmark/include/benchmark/benchmark.h"


int find_the_answer_to_ltuae()
{
	//do not modify
	std::this_thread::sleep_for(std::chrono::seconds(1));
	return 42;
}

void unoptimized(benchmark::State& state)
{
	for (auto _ : state)
	{
		//do not modify
		constexpr int NUMBER_OF_THREADS = 4;
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
		//do not modify
		constexpr int NUMBER_OF_THREADS = 4;
		for (int i = 0; i < NUMBER_OF_THREADS; ++i)
		{
			std::async(std::launch::async, find_the_answer_to_ltuae);
		}
	}
}

BENCHMARK(unoptimized)->Iterations(1);
BENCHMARK(optimized)->Iterations(1);