#ifndef Utilities_HeaderPlusPlus
#define Utilities_HeaderPlusPlus

#include <utility>

namespace util
{
	/**
	 * Run child_func in a separate process, with
	 * the returned int being the write end of
	 * a pipe and the int passed to child_func
	 * being the read end of the pipe. The return
	 * of child_func is the exit code of the
	 * process.
	 */
	int/*write*/ split(int (&child_func)(int read));
	/**
	 * Run child_func in a eparate process, with
	 * the returned pair of ints being the write
	 * and read ends of two pipes and the two
	 * ints passed to child_func being the read
	 * and write ends of the two pipes. The
	 * return of child_func is the exit code of
	 * the process.
	 */
	std::pair<int/*write*/, int/*read*/> split(int (&child_func)(int/*read*/, int/*write*/));
	/**
	 * Fork the process and create a ring with
	 * the parent and child standard input and
	 * standard output streams. The returned
	 * value is the value returned by fork.
	 */
	int split();
}

#endif
