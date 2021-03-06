#include <benchmark/benchmark.h>
#include "OptimizedThreadSafeQueue.h"
#include "SimpleThreadSafeQueue.h"
#include "JoinableThread.h"
#include <thread>

// Do not modify
template <class T>
void bench_body(benchmark::State& state)
{
	srand(0);
	for (auto _ : state)
	{
		T queue;
		JoinableThread<std::thread> t1([&] {
			for (int i = 0; i < state.range(); ++i)
			{
				queue.push(rand());
			}
		});
		JoinableThread<std::thread> t2([&] {
			for (int i = 0; i < state.range(); ++i)
			{
				queue.wait_and_pop();
			}
		});
	}
}

void simple(benchmark::State& state)
{
	bench_body<SimpleThreadSafeQueue<int>>(state);
}

void optimized(benchmark::State& state)
{
	bench_body<optimized_thread_safe_queue<int>>(state);
}

BENCHMARK(simple)->Range(8, 8 << 15)->Iterations(25);
BENCHMARK(optimized)->Range(8, 8 << 15)->Iterations(25);
