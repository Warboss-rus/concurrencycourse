#include "hierarchical_mutex.h"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
#include <limits>

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
// Returns the quotient and remainder
template <class T, class U>
pair<int64_t, int64_t> Div2(T divident, U divisor)
{
	static_assert(is_integral_v<T>, "divident must be integral");
	static_assert(is_integral_v<U>, "divisor must be integral");
	using C = common_type_t<T, U>;
	const C dividentC = static_cast<C>(divident);
	const C divisorC = static_cast<C>(divisor);
	return { dividentC / divisorC, dividentC % divisorC };
}

milliseconds RandomTime(milliseconds from, milliseconds to)
{
	thread_local std::mt19937 generator{ std::random_device()() };
	uniform_int_distribution<milliseconds::rep> distribution(from.count(), to.count());

	return milliseconds(distribution(generator));
}

class Logger
{
public:
	static Logger& Get()
	{
		static Logger s_logger;
		return s_logger;
	}

	static void Log(string_view message)
	{
		Logger& logger = Get();
		logger.LogImpl(message);
	}

private:
	Logger()
		: m_startTime(steady_clock::now())
	{
	}

	void LogImpl(string_view message) const
	{
		cout << FormatUptime() << " " << message << endl;
	}

	string FormatUptime() const
	{
		const milliseconds uptime = GetUptime();
		auto [uptimeSec, uptimeMsec] = Div2(uptime.count(), 1000);
		char buf[128] = { 0 };
		snprintf(buf, std::size(buf), "[%d.%03ds]", static_cast<int>(uptimeSec), static_cast<int>(uptimeMsec));

		return buf;
	}

	milliseconds GetUptime() const
	{
		return duration_cast<milliseconds>(steady_clock::now() - m_startTime);
	}

	steady_clock::time_point m_startTime;
};

void log(const string& message)
{
	cout << message << endl;
}

class Table
{
public:
	explicit Table(size_t seatCount)
		: m_forks(seatCount)
	{
	}

	class Seat
	{
	public:
		Seat(vector<mutex>& forks, size_t fork1, size_t fork2)
			: m_forks(forks)
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
			Logger::Log("requested fork #" + std::to_string(index));
			return m_forks.get().at(index);
		}

		std::reference_wrapper<vector<mutex>> m_forks;
		size_t m_fork1;
		size_t m_fork2;
	};

	Seat GetSeat(size_t index)
	{
		return Seat(m_forks, index % m_forks.size(), (index + 1) % m_forks.size());
	}

private:
	vector<mutex> m_forks;
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
					std::unique_lock lock1(seat.GetFork1());
					this_thread::sleep_for(RandomTime(50ms, 100ms));

					std::unique_lock lock2(seat.GetFork2());
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

	std::this_thread::sleep_for(600s);
}
