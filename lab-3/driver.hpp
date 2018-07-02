#ifndef DRIVER_HH
#define DRIVER_HH

#include <string>

#include "ast_node.hpp"
#include "parser.h"


// Tell Flex the lexer's prototype ...
# define YY_DECL yy::go_parser::symbol_type yylex (go_driver& driver)
// ... and declare it for the parser's sake.
YY_DECL;

class go_driver
{
public:
	// name of input file for parsing
	std::string file;

	ast_node tree;

	// whether parser/scanner traces should be shown
	bool trace_scanning, trace_parsing;

	// setup and teardown functions for scanner
	void scan_begin();
	void scan_end();

	go_driver();
	virtual ~go_driver();

	/* returns 1 if file denoted by fname was parsed successfully, 0 otherwise. */
	int parse(const std::string& fname);

	/* wrapper for private function of the same name */
	int print_ast();

	// Error handling.
	void error(const yy::location& l, const std::string& m);
	void error(const std::string& m);

private:
	/* returns 1 if the AST was printed successfully, 0 otherwise. */
	int print_ast(ast_node *node, int indent);
};
#endif
