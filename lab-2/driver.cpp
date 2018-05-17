#include <unistd.h>

#include <cstring>
#include <iostream>

#include "driver.hpp"


/* command line option flags */

#define	LONG_OPT_TRACE_PARSING		"--parser-traces"
#define	SHORT_OPT_TRACE_PARSING		"-p"
#define	LONG_OPT_TRACE_SCANNING		"--scanner-traces"
#define	SHORT_OPT_TRACE_SCANNING	"-s"

/* constructor/destructor */
go_driver::go_driver()
{
	trace_scanning = false;
	trace_parsing = false;
}

go_driver::~go_driver()
{
}

/* returns 0 if file denoted by fname was parsed successfully, 1 otherwise. */
int go_driver::parse(const std::string &fname)
{
  file = fname;
  scan_begin();
  yy::go_parser parser(*this);
  parser.set_debug_level(trace_parsing);
  int res = parser.parse();
  scan_end();
  return res;
}

/* wrapper for private function of the same name */
int go_driver::print_ast()
{
	return print_ast(&tree, 0);
}

/* returns 1 if the AST was printed successfully, 0 otherwise. */
int go_driver::print_ast(ast_node *node, int indent)
{
	if (!node)
	{
		return 0;
	}

	for (int i = 0; i < indent; i++)
	{
		std::cout << "\t";
	}

	switch (node->type)
	{
		case ast_undefined:
			std::cout << "undefined";
			break;
		case ast_string_literal:
			std::cout << "string literal";
			break;
		case ast_identifier:
			std::cout << "identifier";
			break;
		case ast_import_statement:
			std::cout << "import statements";
			break;
		case ast_import_item:
			std::cout << "import item";
			break;
		case ast_package_statement:
			std::cout << "package statement";
			break;
		case ast_import:
			std::cout << "import";
			break;
		case ast_package:
			std::cout << "package";
			break;
		case ast_root:
			std::cout << "root";
			break;
		default:
			std::cout << "undefined";
			break;
	}
	std::cout << std::endl;

	for(std::vector<int>::size_type i = 0; i != node->children.size(); i++)
	{
		print_ast(&node->children[i], indent + 1);
	}


	return 1;
}

/* prints an error message including the related location in the input file */
void go_driver::error(const yy::location& l, const std::string& m)
{
  std::cerr << l << ": " << m << std::endl;
}

/* prints an error message */
void go_driver::error(const std::string& m)
{
  std::cerr << m << std::endl;
}

/* prints a program usage message */
void printUsage(std::ostream& outputStream, char *programName)
{
	outputStream
		<< "Usage: " << programName << " [OPTION]... [FILE]..." << std::endl
		<< std::endl
		<< "\t-p, --parser-traces" << std::endl
		<< "\t\tInclude parser traces" << std::endl
		<< "\t-s, --scanner-traces" << std::endl
		<< "\t\tPrint scanner traces" << std::endl;
}

int main(int argc, char **argv)
{
	go_driver driver;
	int i = 1;

	while (i < argc)
	{
		if ( !strcmp(argv[i], SHORT_OPT_TRACE_PARSING) || !strcmp(argv[i], LONG_OPT_TRACE_PARSING))
		{
			driver.trace_parsing = true;
		}
		else if (!strcmp(argv[i], SHORT_OPT_TRACE_SCANNING) || !strcmp(argv[i], LONG_OPT_TRACE_SCANNING))
		{
			driver.trace_scanning = true;
		}
		else if (!driver.parse(argv[i]))
		{
			// argument is a file to parse

			// print resulting tree
		 	driver.print_ast();
		}
		else
		{
			// argument meaning is unknown
			std::cerr << "Unrecognised option: " << argv[i] << std::endl;
			printUsage(std::cerr, argv[0]);
			return 1;
		}

		i++;
	}

	// check if input was piped into the program via stdin
	if (!isatty(STDIN_FILENO))
	{
		if (!driver.parse("-"))
		{
			// print resulting tree
		 	driver.print_ast();
		}
	}

	return 0;
}
