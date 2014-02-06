#include <iostream>
#include <unistd.h>

int main()
{
	std::cout << "About to fork" << std::endl;
	int v = 0;
	if(fork() == 0)
	{
		std::cout << "In child process" << std::endl;
		for(int i = 0; i < 5; ++i)
		{
			++v;
			std::cout << "[Child] j = " << v << std::endl;
		}
	}
	else
	{
		std::cout << "In parent process, even if fork failed" << std::endl;
		for(int i = 0; i < 3; ++i)
		{
			++v;
			std::cout << "[Parent] j = " << v << std::endl;
		}
	}
}
