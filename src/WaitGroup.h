#pragma once
#include <future>

class WaitGroup
{
public:
	WaitGroup()
		: m_future(m_promise.get_future())
	{
	}

	void add_task(unsigned count = 1)
	{
		m_taskCount.fetch_add(count, std::memory_order_relaxed);
	}

	void end_task()
	{
		if (1 == m_taskCount.fetch_sub(1, std::memory_order_relaxed))
		{
			m_promise.set_value();
		}
	}

	void wait_all()
	{
		m_future.get();
	}

private:
	std::promise<void> m_promise;
	std::future<void> m_future;
	std::atomic<unsigned> m_taskCount{ 0 };
};