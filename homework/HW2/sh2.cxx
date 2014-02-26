#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <memory>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/fcntl.h>

std::istream &next_token(std::istream &is, std::string &t)
{
	std::string token;
	if(is >> token && token.size() >= 1 && token[0] == '"')
	{
		token.erase(std::begin(token));
		std::string rest;
		while((token[token.size()-1] != '"'
		|| (token.size() >= 2 && token[token.size()-2] == '\\'))
		&& std::getline(is, rest, '"'))
		{
			token += rest + '"';
			rest.clear();
		}
		token.erase(std::end(token)-1);
	}
	return t = std::move(token), is;
}

enum struct ShellOp
{
	And, Or, Pipe, Infile, Outfile, Append
};

int main()
{
	std::string command;
	while(std::cout << "sh2 % " << std::flush
	   && std::getline(std::cin, command))
	{
		std::vector<std::vector<std::string>> procs {{}};
		std::vector<ShellOp> ops;
		bool w = true;
		{
			decltype(procs)::value_type args;
			{
				std::istringstream iss {command};
				std::string arg;
				while(next_token(iss, arg))
				{
					args.push_back(arg);
				}
			}
			if(args.back() == "&")
			{
				w = false;
				args.pop_back();
			}
			for(auto const &arg : args)
			{
				if(arg == "&&")
				{
					procs.push_back({});
					ops.push_back(ShellOp::And);
				}
				else if(arg == "||")
				{
					procs.push_back({});
					ops.push_back(ShellOp::Or);
				}
				else if(arg == "|")
				{
					procs.push_back({});
					ops.push_back(ShellOp::Pipe);
				}
				else if(arg == "<")
				{
					procs.push_back({});
					ops.push_back(ShellOp::Infile);
				}
				else if(arg == ">")
				{
					procs.push_back({});
					ops.push_back(ShellOp::Outfile);
				}
				else if(arg == ">>")
				{
					procs.push_back({});
					ops.push_back(ShellOp::Append);
				}
				else
				{
					procs.back().push_back(arg);
				}
			}
		}
		int p = fork();
		if(p == 0) //if child
		{
			for(std::size_t i = 0; i < procs.size(); ++i)
			{
				if(i < ops.size() && ops[i] == ShellOp::Infile)
				{
					int fd = open(procs.at(i+1).front().c_str(), O_RDONLY);
					if(fd == -1)
					{
						std::exit(errno);
					}
					dup2(fd, 0);
					ops.erase(std::begin(ops)+i), procs.erase(std::begin(procs)+i+1);
				}
				if(i < ops.size() && ops[i] == ShellOp::Outfile)
				{
					int fd = open(procs.at(i+1).front().c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
					if(fd == -1)
					{
						std::exit(errno);
					}
					dup2(fd, 1);
					ops.erase(std::begin(ops)+i), procs.erase(std::begin(procs)+i+1);
				}
				if(i < ops.size() && ops[i] == ShellOp::Append)
				{
					int fd = open(procs.at(i+1).front().c_str(), O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);
					if(fd == -1)
					{
						std::exit(errno);
					}
					dup2(fd, 1);
					ops.erase(std::begin(ops)+i), procs.erase(std::begin(procs)+i+1);
				}
				int io[2];
				if(i < ops.size() && ops[i] == ShellOp::Pipe)
				{
					pipe(io);
				}
				int pid = fork();
				if(pid == 0) //if child
				{
					auto &args = procs[i];
					std::unique_ptr<char *[]> rawargs {new char *[args.size()+1]};
					rawargs[args.size()] = nullptr;
					for(std::size_t i = 0; i < args.size(); ++i)
					{
						rawargs[i] = &(args[i][0]);
					}
					if(i < ops.size() && ops[i] == ShellOp::Pipe)
					{
						dup2(io[1], 1);
						close(io[1]);
						close(io[0]);
					}
					std::exit(execvp(args.front().c_str(), rawargs.get()));
				}
				else if(pid > 0) //if parent
				{
					if(i < ops.size() && ops[i] == ShellOp::Pipe)
					{
						dup2(io[0], 0);
						close(io[0]);
						close(io[1]);
					}
					int s {};
					if(waitpid(pid, &s, 0) == pid)
					{
						if(i < ops.size())
						{
							if((ops[i] == ShellOp::And
							||  ops[i] == ShellOp::Pipe)
							&& WEXITSTATUS(s) != EXIT_SUCCESS)
							{
								std::exit(WEXITSTATUS(s));
							}
							if(ops[i] == ShellOp::Or
							&& WEXITSTATUS(s) == EXIT_SUCCESS)
							{
								std::exit(EXIT_SUCCESS);
							}
						}
						else
						{
							std::exit(WEXITSTATUS(s));
						}
					}
				}
			}
		}
		else if(p > 0) //if parent
		{
			int s {};
			if(w && waitpid(p, &s, 0) == p)
			{
				auto exit_code = WEXITSTATUS(s);
				std::cout << std::endl
						  << "Process completed, exit code "
						  << exit_code
						  << (exit_code == EXIT_SUCCESS ? " (success)" : " (failure)")
						  << std::endl;
			}
			else if(!w)
			{
				std::cout << "Process running in background" << std::endl;
			}
		}
	}
}
