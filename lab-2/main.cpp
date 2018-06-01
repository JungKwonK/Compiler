#include "parser.h"
#include "lexer.h"

#include <errno.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>

void printUsage(std::ostream& outputStream, char *programName)
{
	outputStream << "Usage: " << programName << " [FILE]" << std::endl;
}

int main(int argc, char **argv)
{
	yydebug = 0;
	
	//yytext
	//yyleng
	//yylineno

	if (argc > 2)
	{
		printUsage(std::cerr, argv[0]);
	}

	// check if input was piped into the program via stdin
	if (!isatty(STDIN_FILENO))
	{
		// check if the user also specified an input file
		if (argc == 2)
		{
			std::cerr << "Detected input from stdin, ignoring file: \"" << argv[1] << "\"" << std::endl;
		}

		// set input source to stdin
		yyin = stdin;
	}
	else
	{
		// check if the user specified an input file
		if (argc == 2)
		{
			char *fname = argv[1];
			FILE *fp;

			if (!(fp = fopen(fname, "rb")))
			{
				std::cerr << "'" << fname << "': Couldn't open file" << std::endl;
				exit(errno);
			}
			
			// set input source to file
			yyin = fp;
		}
		else
		{
			printUsage(std::cerr, argv[0]);
			std::cerr << "No input from stdin, and no file specified, exiting..." << std::endl;
			return 1;
		}
	}

	// send output to stdout
	yyout = stdout;

	yyparse();

	return 0;
}
