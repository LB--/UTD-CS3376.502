#include <iostream>
#include <unistd.h>

int main()
{
	std::cout << "Parent is " << getpid() << std::endl;
	fork(); fork();
	std::cout << "(Who am I if I'm " << getpid() << "?)" << std::endl;
}
