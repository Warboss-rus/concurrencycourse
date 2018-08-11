#pragma once
#include <deque>
#include <mutex>
#include <condition_variable>

template <class T>
class SimpleThreadSafeQueue
{
private:
	std::deque<T> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_condVariable;

public:
	void push(T&& newValue)
	{
		std::lock_guard lk(m_mutex);
		m_queue.emplace_back(std::move(newValue));
		m_condVariable.notify_one();
	}

	T wait_and_pop()
	{
		std::unique_lock lk(m_mutex);
		if (m_queue.empty())
		{
			m_condVariable.wait(lk, [&] { return !m_queue.empty(); });
		}
		T value = std::move(m_queue.front());
		m_queue.pop_front();
		return std::move(value);
	}

	bool try_pop(T& val)
	{
		std::lock_guard lk(m_mutex);
		if (!m_queue.empty())
		{
			val = std::move(m_queue.front());
			m_queue.pop_front();
			return true;
		}
		return false;
	}
};