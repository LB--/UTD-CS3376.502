#include <iostream>
#include <unistd.h>

int main()
{
	std::cout << "About to fork" << std::endl;
	int i = fork();
	std::cout << "fork returned " << i << std::endl;
}
