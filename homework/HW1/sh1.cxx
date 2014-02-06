#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <unistd.h>

int main()
{
	std::string command;
	while(std::cout << "sh1 % " << std::flush
	   && std::getline(std::cin, command))
	{
		if(fork() == 0) //if child
		{
			std::istringstream iss (command);
			std::string file, arg;
			iss >> file;
			std::vector<std::string> args;
			args.push_back(file);
			while(iss >> arg)
			{
				args.push_back(arg);
			}
			char const **rawargs = new char const *[args.size()+1];
			rawargs[args.size()] = 0;
			for(std::vector<std::string>::size_type i = 0; i < args.size(); ++i)
			{
				rawargs[i] = args[i].c_str();
			}
			execvp(file.c_str(), const_cast<char **>(rawargs));
			delete[] rawargs;
			break;
		}
	}
}
