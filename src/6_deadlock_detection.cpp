#include "hierarchical_mutex.h"
#include "Logger.h"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>
#include <list>
#include <mutex>
#include <random>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <utility>

/**
 * TODO: выполните два задания
 * 1) исправьте проблему перемешивания вывода из разных потоков при записи в cout, изменив как можно меньше кода
 * 2) с помощью hierarchical_mutex найдите место возникновения deadlock
 * 3) исправьте deadlock, изменив как можно меньше кода
 */

using namespace std;
using namespace std::chrono;

namespace
{

milliseconds RandomTime(milliseconds from, milliseconds to)
{
	thread_local mt19937 generator{ random_device()() };
	uniform_int_distribution<milliseconds::rep> distribution(from.count(), to.count());

	return milliseconds(distribution(generator));
}

class Table
{
public:
	explicit Table(size_t seatCount)
	{
		for (size_t i = 0; i < seatCount; ++i)
		{
			m_forks.emplace_back();
		}
	}

	class Seat
	{
	public:
		Seat(list<mutex>& forks, size_t fork1, size_t fork2)
			: m_forks(&forks)
			, m_fork1(fork1)
			, m_fork2(fork2)
		{
		}

		mutex& GetFork1()
		{
			return GetFork(m_fork1);
		}

		mutex& GetFork2()
		{
			return GetFork(m_fork2);
		}

	private:
		mutex& GetFork(size_t index)
		{
			Logger::Log("requested fork #" + to_string(index));
			auto it = m_forks->begin();
			std::advance(it, index);
			return *it;
		}

		list<mutex>* m_forks = nullptr;
		size_t m_fork1;
		size_t m_fork2;
	};

	Seat GetSeat(size_t index)
	{
		return Seat(m_forks, index % m_forks.size(), (index + 1) % m_forks.size());
	}

private:
	list<mutex> m_forks;
};

class Philosopher
{
public:
	Philosopher(string name)
		: m_name(move(name))
	{
	}

	Philosopher(const Philosopher&) = delete;
	Philosopher& operator=(const Philosopher&) = delete;
	Philosopher(Philosopher&&) = delete;
	Philosopher& operator=(Philosopher&&) = delete;

	~Philosopher()
	{
		m_cancelled = true;
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

	void Start(Table::Seat seat)
	{
		m_thread = thread([this, seat]() mutable {
			while (!m_cancelled)
			{
				Logger::Log(m_name + " thinks...");
				this_thread::sleep_for(RandomTime(50ms, 500ms));
				Logger::Log(m_name + " going to eat");
				{
					unique_lock lock1(seat.GetFork1());
					this_thread::sleep_for(RandomTime(50ms, 100ms));

					unique_lock lock2(seat.GetFork2());
					Logger::Log(m_name + " eats..");
					this_thread::sleep_for(RandomTime(50ms, 125ms));
				}
			}
		});
	}

private:
	atomic_bool m_cancelled{ false };
	thread m_thread;
	string m_name;
};
} // namespace

int main()
{
	constexpr size_t seatCount = 3;

	Table table(seatCount);

	Philosopher kant("Immanuel Kant");
	kant.Start(table.GetSeat(0));

	Philosopher spinoza("Baruch Spinoza");
	spinoza.Start(table.GetSeat(1));

	Philosopher bacon("Francis Bacon");
	bacon.Start(table.GetSeat(2));

	// Число мест и философов уменьшено, чтобы увеличить вероятность deadlock
#if 0
    Philosopher descartes("Rene Descartes");
    descartes.Start(table.GetSeat(3));

    Philosopher hegel("Georg Wilhelm Friedrich Hegel");
    hegel.Start(table.GetSeat(4));
#endif

	this_thread::sleep_for(60s);
}
