#include <thread>
#include <stdexcept>

// do not modify
void thread_proc()
{
	throw std::runtime_error("something went wrong");
}

int main()
{
	std::thread t(thread_proc);
	throw std::runtime_error("something went wrong");
	t.join();
	return 0;
}

