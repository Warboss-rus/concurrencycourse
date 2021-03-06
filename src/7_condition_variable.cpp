#include "JoinableThread.h"
#include <iostream>
#include <thread>

constexpr int COUNT = 10;

int main()
{
	JoinableThread<std::thread> t1([] {
		for (int i = 0; i < COUNT; ++i)
		{
			std::cout << "ping" << std::endl;
		}
	});
	JoinableThread<std::thread> t2([] {
		for (int i = 0; i < COUNT; ++i)
		{
			std::cout << "pong" << std::endl;
		}
	});

	return 0;
}
