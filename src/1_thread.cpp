#include <thread>

int main()
{
	std::thread t([] {
		//throw std::runtime_error("something went wrong");
	});
	//throw std::runtime_error("something went wrong");
	t.join();
	return 0;
}

