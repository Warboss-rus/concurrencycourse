#include <iostream>
#include <thread>
#include "JoinableThread.h"

const std::string LOREM_IPSUM = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

int main()
{
	auto threadProc = [] {
		for (size_t i = 0; i < LOREM_IPSUM.size(); ++i)
		{
			std::cout << LOREM_IPSUM[i];
		}
		std::cout << std::endl;
	};
	
	JoinableThread<std::thread> t1(threadProc);
	JoinableThread<std::thread> t2(threadProc);
	
	getchar();
	return 0;
}

