#ifndef HW3_Mtrix_Common_HeaderXX
#define HW3_Mtrix_Common_HeaderXX

#include <vector>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef std::vector<double> Column_t;
typedef std::vector<Column_t> Matrix_t;
int const MatrixPort = 6127;

inline Matrix_t receiveMatrix(int sock)
{
	std::size_t x, y;
	if(recv(sock, &x, sizeof(x), MSG_WAITALL) == sizeof(x)
	&& recv(sock, &y, sizeof(y), MSG_WAITALL) == sizeof(y))
	{
		Matrix_t matrix (x, Column_t(y));
		for(Matrix_t::iterator it = matrix.begin(); it != matrix.end(); ++it)
		{
			for(Column_t::iterator jt = it->begin(); jt != it->end(); ++jt)
			{
				if(recv(sock, &*jt, sizeof(*jt), MSG_WAITALL) != sizeof(*jt))
				{
					return Matrix_t();
				}
			}
		}
		return matrix;
	}
	return Matrix_t();
}
inline void sendMatrix(int sock, Matrix_t const &m)
{
	std::size_t x = m.size();
	std::size_t y = (m.size() > 0 ? m[0].size() : 0);
	send(sock, &x, sizeof(x), 0);
	send(sock, &y, sizeof(y), 0);
	for(Matrix_t::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		for(Column_t::const_iterator jt = it->begin(); jt != it->end(); ++jt)
		{
			send(sock, &*jt, sizeof(*jt), 0);
		}
	}
}

#endif
