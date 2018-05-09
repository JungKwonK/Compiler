#ifndef UTIL_HPP
#define UTIL_HPP

namespace util
{
	#define UTIL_STDIN_BUF_LEN   8192    // initial length in bytes of the buffer to hold input from stdin

	/* Returns contents of the file denoted by fname  */
	char * readFile(const char *fname);

	/* Returns contents of stdin up to EOF */
	char * readStdin();
}
#endif
