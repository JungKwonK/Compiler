#ifndef PARSER_HPP
#define PARSER_HPP

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <stdexcept>
#include <vector>

#define	PARSER_STDIN_BUF_LEN	8192	// initial length in bytes of parser stdin buffer
#define PARSER_EXCEP_MSG_LEN	64		// max length in bytes of parser exception message

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
   AstImportStatements,
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
	
	// offset of the token in bytes from the beginning of the input buffer
	size_t offset;
} Token;

class AstNode
{
public:
	AstNodeType type;
	Token tok;

	std::vector<AstNode*> children;
	
	AstNode();
	AstNode(AstNodeType t);
	~AstNode();
	void addChild(AstNode *node);
};

class Parser
{
private:

	/*static const char **keywordStrings = {
		"import",
		"package"
	};
	
	static const TokenType* keywordTokenTypes = {
		Import,
		Package
	};
	
	static const char **operatorStrings = {
		"(",
		")"
	};
	
	static const TokenType *operatorTokenTypes = {
		OpenBracket,
		CloseBracket
	};*/

	// maps strings to their associated token type
	static const std::map<const char*, TokenType> keywords;
	static const std::map<const char*, TokenType> operators;

	std::map<const char*, TokenType>::const_iterator iter;

	AstNode *ast;

	// data about current token
	Token currentToken; 

	// start of input buffer
	const char *start;

	// current position in input buffer
	const char *input;

	/* These functions return a map that maps strings to their token type */
	static const std::map<const char*, TokenType> createKeywordMap();
	static const std::map<const char*, TokenType> createOperatorMap();

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

	/* functions that represent our grammar productions */
	AstNode* buildAst();

	int printAst(AstNode *node, int indent);

	AstNode* packageStatement();

	AstNode* importStatements();

	AstNode* importStatement();

	AstNode* imports();

public:
	void parse(const char *str);

	/* Returns 1 if the Ast was printed successfully, 0 otherwise */
	int printAst();
};

class ParserException : public std::exception
{
	char msg[PARSER_EXCEP_MSG_LEN];
	const char tok;
	const size_t pos;

	public:
		ParserException(char token, size_t position): tok(token), pos(position)
		{
			if (isprint(tok))
			{
				snprintf(msg, sizeof(msg) / sizeof(msg[0]), "Unexpected token '%c' at position %zu.", tok, pos);
			}
			else
			{
				snprintf(msg, sizeof(msg) / sizeof(msg[0]), "Unexpected token \\x%02x at position %zu.", (unsigned char) tok, pos);
			}
		}

		virtual const char *what() const throw()
		{
			return msg;
		}
};
#endif
