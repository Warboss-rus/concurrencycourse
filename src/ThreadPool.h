#pragma once
#include "JoinableThread.h"
#include "SimpleThreadSafeQueue.h"
#include <atomic>
#include <future>

class thread_pool
{
	std::atomic_bool done = ATOMIC_VAR_INIT(false);
	SimpleThreadSafeQueue<std::packaged_task<void()>> work_queue;
	std::vector<JoinableThread<std::thread>> threads;

	void worker_thread()
	{
		while (!done)
		{
			std::packaged_task<void()> task;
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
	std::future<void> submit(FunctionType f)
	{
		std::packaged_task<void()> task(f);
		auto future = task.get_future();
		work_queue.push(std::move(task));
		return std::move(future);
	}

	void run_pending_task()
	{
		std::packaged_task<void()> task;
		if (work_queue.try_pop(task))
		{
			task();
		}
	}
};