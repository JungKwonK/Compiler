#ifndef PARSER_H
#define PARSER_H

#include <cstdlib>
#include <stdexcept>

#define PIPE_BUFFER_SIZE 8192

using namespace std;

enum TokenType
{
	Import,
	Package,
	OpenBracket,
	CloseBracket,
	Identifier,
	EndOfText,
	StringLiteral,
	Error
};

enum ASTNodeType 
{
   ASTUndefined,
   ASTStringLiteral,
   ASTIdentifier,
   ASTImportItem,
   ASTImportStatements,
   ASTPackageStatement,
   ASTImport,
   ASTPackage,
   ASTRoot
};

typedef struct Token
{
	TokenType type = Error;
	char value[1024] = {0};
	size_t length = 0;
} Token;

class ParserException : public std::runtime_error
{
	int pos;

	public:
		ParserException(std::string const& message, int p):
			std::runtime_error(message), pos(p)
			{
			}
};
#endif
