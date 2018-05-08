#ifndef PARSER_HPP
#define PARSER_HPP

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <stdexcept>
#include <vector>

#define PARSER_EXCEP_MSG_LEN    64      // max length in bytes of parser exception message

enum TokenType
{
	Import,
	Package,
	OpenBracket,
	CloseBracket,
	Identifier,
	EndOfFile,
	StringLiteral,
	Undefined
};

enum AstNodeType
{
   AstUndefined,
   AstStringLiteral,
   AstIdentifier,
   AstImportItem,
   AstImportStatement,
   AstPackageStatement,
   AstImport,
   AstPackage,
   AstRoot
};

typedef struct Token
{
	TokenType type;

	// length of the token in bytes
	size_t length;

	// line and column numbers, zero-based
	size_t line;
	size_t column;
} Token;

class AstNode
{
public:
	AstNodeType type;
	Token tok;

	std::vector<AstNode *> children;

	AstNode();
	AstNode(AstNodeType t);
	~AstNode();
	void addChild(AstNode *node);
};

class Parser
{
public:
	/* Tokenises and generates an abstract syntax tree
	 * for the input beginning at str
	 */
	void parse(const char *str);

	/* Returns 1 if the Ast was printed successfully, 0 otherwise */
	int printAst();

	/* prints the token beginning at line:column to the specified output stream
	 */
	void printToken(std::ostream &out, Token tok);

private:
	// maps strings to their associated token type
	static const std::map<const char *, TokenType> keywords;
	static const std::map<const char *, TokenType> operators;

	std::map<const char *, TokenType>::const_iterator iter;

	AstNode *ast;

	// data about current token
	Token currentToken;

	// start of input buffer
	const char *start;

	// current position in input buffer
	const char *input;

	// vector holding pointers to the start of every line of input
	std::vector<const char *> lines;

	// current line of input, where 0 would mean the first line
	size_t currentLine;

	/* These functions return a map that maps strings to their token type */
	static const std::map<const char *, TokenType> createKeywordMap();
	static const std::map<const char *, TokenType> createOperatorMap();

	/* Parses the token at input as a keyword defined by str.
	 * Returns the length of the token if parsing was successful, 0 otherwise.
	 */
	size_t parseKeyword(const char *input, const char *str);

	/* Parses the token at input as an operator defined by str.
	 * Returns the length of the token if parsing was successful, 0 otherwise.
	 */
	size_t parseOperator(const char *input, const char *str);

	/* Parses the token at input as a string literal.
	 * Returns the length of the token if parsing was successful, 0 otherwise.
	 */
	size_t parseStringLiteral(const char *input);

	/* Parses the token at input as an identifier.
	 * Returns the length of the token if parsing was successful, 0 otherwise.
	 */
	size_t parseIdentifier(const char *input);

	/* Returns 1 if the next token was parsed successfully, 0 otherwise */
	int parseNextToken();

	/* The parser should call this every time '\n' is encountered during tokenisation */
	void newLine(const char *where);

	/* builds an abstract syntax tree */
	AstNode * buildAst();

	/* output the abtract syntax tree in text form */
	int printAst(AstNode *node, int indent);

	/* grammar productions */

	AstNode * packageStatement();

	AstNode * importStatements();

	AstNode * importStatement();

	AstNode * imports();
};

/* Currently used to report unrecognised tokens during tokenisation */
class ParserException : public std::exception
{
public:
	ParserException(Token t);

	/* Returns a copy of the token to the caller */
	Token getToken();

	virtual const char *what() const throw();

private:
	char msg[PARSER_EXCEP_MSG_LEN];

	// line and column numbers, zero-based
	const Token tok;
};
#endif
