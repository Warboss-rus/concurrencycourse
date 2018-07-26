#pragma once
#include "SimpleThreadSafeQueue.h"
#include "JoinableThread.h"
#include <atomic>

class thread_pool
{
	std::atomic_bool done = ATOMIC_VAR_INIT(false);
	SimpleThreadSafeQueue<std::function<void()>> work_queue;
	std::vector<JoinableThread<std::thread>> threads;

	void worker_thread()
	{
		while (!done)
		{
			if (auto task = work_queue.wait_and_pop())
			{
				task();
			}
		}
	}

public:
	thread_pool()
	{
		unsigned const thread_count = std::thread::hardware_concurrency();
		try
		{
			for (unsigned i = 0; i < thread_count; ++i)
			{
				threads.push_back(std::thread(&thread_pool::worker_thread, this));
			}
		}
		catch (...)
		{
			done = true;
			throw;
		}
	}

	~thread_pool()
	{
		done = true;
	}

	template <typename FunctionType>
	void submit(FunctionType f)
	{
		work_queue.push(std::function<void()>(f));
	}
};