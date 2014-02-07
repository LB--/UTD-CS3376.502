#include <iostream>
#include <string>

#include "util.hxx"

#include <unistd.h>

int child_p2c(int in)
{
	std::cout << "[Child] After split, before read." << std::endl;
	std::string msg;
	{
		char c;
		while(read(in, &c, 1) != 0) msg += c;
	}
	close(in);
	std::cout << "[Child] After split, after read: \"" << msg << "\"" << std::endl;
	return 0;
}
int child_both(int in, int out)
{
	std::cout << "[Child] After split, before read, before write." << std::endl;
	std::string msg;
	{
		char c;
		while(read(in, &c, 1) != 0) msg += c;
	}
	close(in);
	std::cout << "[Child] After split, after read: \"" << msg << "\", before write." << std::endl;
	msg = "From Child to Parent";
	write(out, msg.c_str(), msg.size()+1);
	close(out);
	std::cout << "[Child] After split, after read, after write." << std::endl;
	return 0;
}

int main(int nargs, char const *const *args)
{
	if(nargs != 2)
	{
		std::cerr << "Argument must be either p2c, both, or ring." << std::endl;
		return -1;
	}
	std::string arg = args[1];
	if(arg == "p2c")
	{
		std::cout << "Before split." << std::endl;
		int out = util::split(child_p2c);
		std::cout << "[Parent] After split, before write." << std::endl;
		std::string msg = "From Parent to Child";
		write(out, msg.c_str(), msg.size()+1);
		close(out);
		std::cout << "[Parent] After split, after write." << std::endl;
	}
	else if(arg == "both")
	{
		std::cout << "Before split." << std::endl;
		std::pair<int, int> io = util::split(child_both);
		int out = io.first, in = io.second;
		std::cout << "[Parent] After split, before write, before read." << std::endl;
		std::string msg = "From Parent to Child";
		write(out, msg.c_str(), msg.size()+1);
		close(out);
		std::cout << "[Parent] After split, after write, before read." << std::endl;
		msg.clear();
		{
			char c;
			while(read(in, &c, 1) != 0) msg += c;
		}
		close(in);
		std::cout << "[Parent] After split, after write, after read: \"" << msg << "\"" << std::endl;
	}
	else if(arg == "ring")
	{
		//...
	}
	else return -1;
}
