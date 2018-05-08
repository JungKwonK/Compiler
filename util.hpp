#ifndef UTIL_HPP
#define UTIL_HPP

namespace util
{
	/* Returns contents of the file denoted by fname  */
	char * readFile(const char *fname);

	/* Returns contents of stdin up to EOF */
	char * readStdin();
}
#endif
