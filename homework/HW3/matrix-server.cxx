#include <iostream>

#include "matrix-common.hxx"

#include <pthread.h>

void *handle_client(void *p);

int main()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(MatrixPort);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(sock, (sockaddr *)&addr, sizeof(addr));

	listen(sock, 100);

	std::cout << "Running, waiting for connections..." << std::endl;

	pthread_attr_t attr = {};
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	while(true)
	{
		sockaddr_in caddr;
		socklen_t sizeof_caddr = sizeof(caddr);
		int client = accept(sock, (sockaddr *)&caddr, &sizeof_caddr);
		pthread_t thread;
		pthread_create(&thread, &attr, &handle_client, new int(client));
		std::cout << "Connection accepted, waiting for others..." << std::endl;
	}
	pthread_attr_destroy(&attr);
	close(sock);
}

struct MatrixMultParam
{
	Matrix_t const &m1;
	Matrix_t const &m2;
	Matrix_t &m3;
	pthread_mutex_t &mm;
	std::size_t n, m, p, x;
	MatrixMultParam(Matrix_t const &m1, Matrix_t const &m2, Matrix_t &m3, pthread_mutex_t &mm, std::size_t n, std::size_t m, std::size_t p, std::size_t x)
	: m1(m1)
	, m2(m2)
	, m3(m3)
	, mm(mm)
	, n(n)
	, m(m)
	, p(p)
	, x(x)
	{
	}
};

void *matrix_mult(void *p);

void *handle_client(void *ptr)
{
	int *pclient = (int *)ptr;
	int client = *pclient;
	delete pclient, pclient = 0;
	struct RAII { int client; RAII(int client) : client(client) {} ~RAII(){ close(client); } } raii (client);

	Matrix_t m1 = receiveMatrix(client); //n by m
	Matrix_t m2 = receiveMatrix(client); //m by p
	if(m1.size() == 0 || m2.size() == 0 || m1[0].size() != m2.size())
	{
		return 0;
	}
	std::size_t const n = m1.size(), m = m2.size(), p = m2[0].size();
	Matrix_t m3 (n, Column_t(p)); //n by p
	pthread_mutex_t mm = {};
	pthread_mutex_init(&mm, 0);

	pthread_attr_t attr = {};
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	std::vector<pthread_t> threads;
	for(std::size_t i = 0; i < n; ++i)
	{
		pthread_t thread;
		pthread_create(&thread, &attr, &matrix_mult, new MatrixMultParam(m1, m2, m3, mm, n, m, p, i));
		threads.push_back(thread);
	}
	while(threads.size() > 0)
	{
		pthread_join(threads.back(), 0);
		threads.pop_back();
	}

	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&mm);

	sendMatrix(client, m3);

	return 0;
}

void *matrix_mult(void *ptr)
{
	MatrixMultParam *mmp = (MatrixMultParam *)ptr;
	struct RAII { MatrixMultParam *mmp; RAII(MatrixMultParam *mmp) : mmp(mmp) {} ~RAII(){ delete mmp, mmp = 0; } } raii (mmp);

	Matrix_t const &m1 = mmp->m1;
	Matrix_t const &m2 = mmp->m2;
	Matrix_t &m3 = mmp->m3;
	pthread_mutex_t &mm = mmp->mm;
	std::size_t const n = mmp->n;
	std::size_t const m = mmp->m;
	std::size_t const p = mmp->p;
	std::size_t const x = mmp->x; //i,m

	for(std::size_t j = 0; j < p; ++j)
	{
		double v = 0.0;
		for(std::size_t k = 0; k < m; ++k)
		{
			v += m1[x][k] * m2[k][j];
		}
		pthread_mutex_lock(&mm);
		m3[x][j] = v;
		pthread_mutex_unlock(&mm);
	}

	return 0;
}
