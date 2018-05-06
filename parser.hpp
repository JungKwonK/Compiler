#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#define	PARSER_STDIN_BUF_LEN	8192	// initial buffer length in bytes, when reading from stdin
#define PARSER_EXCEP_MSG_LEN	64		// max message length in bytes for parser exceptions

enum TokenType
{
	ImportKeyword,
	PackageKeyword,
	OpenBracket,
	CloseBracket,
	Identifier,
	EndOfFile,
	StringLiteral,
	Undefined
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
	TokenType type;
	
	// length of the token in bytes
	size_t length;
	
	// offset of the token in bytes from the beginning of the input buffer
	size_t offset;
} Token;

class ParserException : public std::exception
{
	char msg[PARSER_EXCEP_MSG_LEN];
	char tok;
	size_t pos;

	public:
		ParserException(char token, size_t position): tok(token), pos(position)
		{
			snprintf(msg, sizeof(msg) / sizeof(msg[0]), "Unexpected token '%c' at position %zu.", tok, pos);
		}

		virtual const char* what() const throw()
		{
			return msg;
		}
};
#endif
