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
	std::atomic<unsigned> m_taskCount{ 0 };
	std::condition_variable m_tasksComplete;
	std::mutex m_tasksCompleteMutex;

	void worker_thread()
	{
		while (!done)
		{
			Task task;
			if (work_queue.try_pop(task))
			{
				task();
				if (1 == m_taskCount.fetch_sub(1, std::memory_order_relaxed))
				{
					std::unique_lock lk(m_tasksCompleteMutex);
					m_tasksComplete.notify_all();
				}
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
		m_taskCount.fetch_add(1, std::memory_order_relaxed);
		work_queue.push(std::move(f));
	}

	void wait_all()
	{
		std::unique_lock lk(m_tasksCompleteMutex);
		m_tasksComplete.wait(lk, [this](){
			return m_taskCount == 0;
		});
	}
};