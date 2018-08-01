#pragma once
#include "JoinableThread.h"
#include "SimpleThreadSafeQueue.h"
#include <atomic>
#include <future>

class thread_pool
{
	using Task = std::function<void()>;

	std::atomic_bool done = ATOMIC_VAR_INIT(false);
	SimpleThreadSafeQueue<Task> work_queue;
	std::vector<JoinableThread<std::thread>> threads;

	void worker_thread()
	{
		while (!done)
		{
			Task task;
			if (work_queue.try_pop(task))
			{
				task();
			}
			else
			{
				std::this_thread::yield();
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
		work_queue.push(std::move(f));
	}
};