#include <thread>
#include <stdexcept>
#include <iostream>

// do not modify
void thread_proc()
{
	throw std::runtime_error("something went wrong");
}

// do not modify
void do_something_on_main_thread()
{
	throw std::runtime_error("something went wrong");
}

int main()
{
	try
	{
		std::thread t(thread_proc);
		do_something_on_main_thread();
		t.join();
	}
	catch (...)
	{
	}
	std::cout << "Everything is OK!\n";
	return 0;
}

