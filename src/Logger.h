#pragma once

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

// Класс Logger - это Singleton, к которому перенаправляются все записи в стандартный поток вывода (stdout).
class Logger
{
public:
	static Logger& Get()
	{
		static Logger s_logger;
		return s_logger;
	}

	static void Log(std::string_view message)
	{
		Logger& logger = Get();
		logger.LogImpl(message);
	}

private:
	Logger()
		: m_startTime(std::chrono::steady_clock::now())
	{
	}

	void LogImpl(std::string_view message)
	{
		std::cout << FormatUptime() << " " << message << std::endl;
	}

	std::string FormatUptime() const
	{
		const std::chrono::milliseconds uptime = GetUptime();
		auto uptimeSec = uptime.count() / 1000;
		auto uptimeMsec = uptime.count() % 1000;
		char buf[128] = { 0 };
		snprintf(buf, std::size(buf), "[%d.%03ds]", static_cast<int>(uptimeSec), static_cast<int>(uptimeMsec));

		return buf;
	}

	std::chrono::milliseconds GetUptime() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_startTime);
	}

	std::chrono::steady_clock::time_point m_startTime;
};
