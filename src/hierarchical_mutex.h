#pragma once

#include "Logger.h"
#include <cstdlib>
#include <mutex>
#include <stdexcept>

//  ласс hierarchical_mutex представл€ет mutex с обнаружением deadlock.
// ќбъекты типа hierarchical_mutex совместно выстраивают иерархию использующих их потоков.
// ƒл€ хранени€ индекса потока в иерархии используетс€ thread_local переменна€.
// Ќачальный индекс потока в иерархии - максимальное число unsigned.
// «атем, при захвате mutex индекс потока становитс€ равным константному индексу mutex,
//  а mutex запоминает предыдущее значение индекса иерархии потока.
// «атем, при освобождении mutex индекс потока становитс€ прежним.
// ≈сли при захвате мьютекса индекс потока (т.е. индекс ранее захватенного mutex)
//  меньше индекса захватываемого mutex, то произошЄл deadlock.
class hierarchical_mutex
{
public:
	explicit hierarchical_mutex(unsigned long value)
		: hierarchy_value(value)
		, previous_hierarchy_value(0)
	{
	}

	void lock()
	{
		check_for_hierarchy_violation();
		internal_mutex.lock();
		update_hierarchy_value();
	}

	void unlock()
	{
		this_thread_hierarchy_value = previous_hierarchy_value;
		internal_mutex.unlock();
	}

	bool try_lock()
	{
		check_for_hierarchy_violation();
		if (!internal_mutex.try_lock())
		{
			return false;
		}

		update_hierarchy_value();
		return true;
	}

private:
	std::mutex internal_mutex;
	unsigned long const hierarchy_value;
	unsigned previous_hierarchy_value;

	static thread_local unsigned long this_thread_hierarchy_value;

	void check_for_hierarchy_violation()
	{
		if (this_thread_hierarchy_value <= hierarchy_value)
		{
#if 1 // “екуща€ стратеги€ обработки deadlock: вызов abort
			Logger::Get().Log("deadlock detected! aborting");
			abort();
#else
			throw std::logic_error("mutex hierarchy violated");
#endif
		}
	}

	void update_hierarchy_value()
	{
		previous_hierarchy_value = this_thread_hierarchy_value;
		this_thread_hierarchy_value = hierarchy_value;
	}
};
