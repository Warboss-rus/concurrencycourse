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
	thread_local mt19937 generator{ random_device()() };
	uniform_int_distribution<milliseconds::rep> distribution(from.count(), to.count());

	return milliseconds(distribution(generator));
}

// Класс Logger - это Singleton, к которому перенаправляются все записи в стандартный поток вывода (stdout).
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

	void LogImpl(string_view message)
	{
		cout << FormatUptime() << " " << message << endl;
	}

	string FormatUptime() const
	{
		const milliseconds uptime = GetUptime();
		auto [uptimeSec, uptimeMsec] = Div2(uptime.count(), 1000);
		char buf[128] = { 0 };
		snprintf(buf, size(buf), "[%d.%03ds]", static_cast<int>(uptimeSec), static_cast<int>(uptimeMsec));

		return buf;
	}

	milliseconds GetUptime() const
	{
		return duration_cast<milliseconds>(steady_clock::now() - m_startTime);
	}

	steady_clock::time_point m_startTime;
};

// Класс hierarchical_mutex представляет mutex с обнаружением deadlock.
// Объекты типа hierarchical_mutex совместно выстраивают иерархию использующих их потоков.
// Для хранения индекса потока в иерархии используется thread_local переменная.
// Начальный индекс потока в иерархии - максимальное число unsigned.
// Затем, при захвате mutex индекс потока становится равным константному индексу mutex,
//  а mutex запоминает предыдущее значение индекса иерархии потока.
// Затем, при освобождении mutex индекс потока становится прежним.
// Если при захвате мьютекса индекс потока (т.е. индекс ранее захватенного mutex)
//  меньше индекса захватываемого mutex, то произошёл deadlock.
class hierarchical_mutex
{
public:
	explicit hierarchical_mutex(unsigned value)
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
	mutex internal_mutex;
	unsigned hierarchy_value;
	unsigned previous_hierarchy_value;

	static thread_local unsigned this_thread_hierarchy_value;

	void check_for_hierarchy_violation()
	{
		if (this_thread_hierarchy_value <= hierarchy_value)
		{
#if 1 // Текущая стратегия обработки deadlock: вызов abort
			Logger::Get().Log("deadlock detected! aborting");
			abort();
#else
			throw logic_error("mutex hierarchy violated");
#endif
		}
	}

	void update_hierarchy_value()
	{
		previous_hierarchy_value = this_thread_hierarchy_value;
		this_thread_hierarchy_value = hierarchy_value;
	}
};

thread_local unsigned hierarchical_mutex::this_thread_hierarchy_value{ (numeric_limits<unsigned>::max)() };

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
