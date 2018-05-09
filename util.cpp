#include "util.hpp"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

#define UTIL_STDIN_BUF_LEN   8192    // initial length in bytes of the buffer to hold input from stdin

namespace util
{
	/* Returns a malloc'ed char array if fname was successfully read, NULL otherwise.
	 * The caller is responsible for freeing the returned pointer */
	char * readFile(const char *fname)
	{
		char *array;
		ssize_t size;

		struct stat finfo;
		int fd;

		if (!(fd = open(fname, O_RDONLY)))
		{
			std::cerr << "'" << fname << "': Couldn't open file" << std::endl;
			return NULL;
		}

		if (fstat(fd, &finfo))
		{
			std::cerr << fname << ": " << std::strerror(errno) << std::endl;
			return NULL;
		}

		if (!S_ISREG(finfo.st_mode))
		{
			std::cerr << "'" << fname << "': Not a regular file" << std::endl;
			return NULL;
		}

		size = finfo.st_size;

		if (!(array = (char *) malloc(size + 1)))
		{
			std::cerr << "'" << fname << "': Not enough memory to read file" << std::endl;
			close(fd);
			return NULL;
		}

		if (read(fd, array, size) != size)
		{
			std::cerr << "'" << fname << "': Couldn't read entire file" << std::endl;
			close(fd);
			free(array);
			return NULL;
		}

		close(fd);

		// null-terminate the string
		array[size] = '\0';

		return array;
	}

	/* Returns a malloc'ed char array if stdin was successfully read, NULL otherwise.
	 * The caller is responsible for freeing the returned pointer */
	char * readStdin()
	{
		char *array;
		size_t size;

		size_t sizeIncrement = UTIL_STDIN_BUF_LEN;

		if (!(array = (char *) malloc(sizeIncrement)))
		{
			std::cerr << "Not enough memory to read input" << std::endl;
			return NULL;
		}

		size = 0;
		while (std::cin >> std::noskipws >> array[size++])
		{
			if (size == sizeof(array) - sizeof(array[0]))
			{
				// allocate more memory every time the buffer is full
				if (!(array = (char *) realloc(array, size + sizeIncrement)))
				{
					std::cerr << "Not enough memory to read input" << std::endl;
					free(array);
					return NULL;
				}

			}
		}

		// null-terminate the string
		array[size - 1] = '\0';

		return array;
	}
}
