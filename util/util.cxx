#include "util.hxx"

#include <cstdlib>

#include <unistd.h>

namespace util
{
	int split(int (&child_func)(int))
	{
		int p2c[2];
		pipe(p2c);
		if(fork() == 0) //child
		{
			close(p2c[1]);
			std::exit(child_func(p2c[0]));
		}
		else //parent
		{
			close(p2c[0]);
			return p2c[1];
		}
		return -1; //this line is never reached
	}
	std::pair<int, int> split(int (&child_func)(int, int))
	{
		int p2c[2];
		pipe(p2c);
		int c2p[2];
		pipe(c2p);
		if(fork() == 0) //child
		{
			close(p2c[1]);
			close(c2p[0]);
			std::exit(child_func(p2c[0], c2p[1]));
		}
		else //parent
		{
			close(p2c[0]);
			close(c2p[1]);
			return std::make_pair(p2c[1], c2p[0]);
		}
		return std::make_pair(-1, -1); //this line is never reached
	}
	int split()
	{
		int fd[2];
		pipe(fd);
		dup2(fd[0], 0);
		dup2(fd[1], 1);
		close(fd[0]);
		close(fd[1]);
		pipe(fd);
		int fr = fork();
		if(fr == 0) //child
		{
			dup2(fd[1], 1);
		}
		else //parent
		{
			dup2(fd[0], 0);
		}
		close(fd[0]);
		close(fd[1]);
		return fr;
	}
}
