#include <iostream>

#include "matrix-common.hxx"

int main()
{
	sockaddr_in saddr = {};
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	saddr.sin_port = htons(MatrixPort);

	int server = socket(AF_INET, SOCK_STREAM, 0);
	if(server < 0)
	{
		std::cerr << "Could not create socket" << std::endl;
		return -1;
	}
	if(connect(server, (sockaddr *)&saddr, sizeof(saddr)) < 0)
	{
		std::cerr << "Could not connect to server" << std::endl;
		return -2;
	}

	std::size_t const n = 4, m = 2, p = 3;
	Matrix_t m1 (n, Column_t(m, 1.0));
	Matrix_t m2 (m, Column_t(p, 1.0));

	sendMatrix(server, m1);
	sendMatrix(server, m2);
	Matrix_t m3 = receiveMatrix(server);
	close(server);
	for(Matrix_t::iterator it = m3.begin(); it != m3.end(); ++it)
	{
		for(Column_t::iterator jt = it->begin(); jt != it->end(); ++jt)
		{
			std::cout << *jt << " " << std::flush;
		}
		std::cout << std::endl;
	}
}
