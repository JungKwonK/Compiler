
#include "parser.hpp"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>

AstNode::AstNode()
{
  type = AstUndefined;
  tok.type = Undefined;
}

AstNode::AstNode(AstNodeType t)
{
  type = t;
  tok.type = Undefined;
}


AstNode::~AstNode()
{
	for(std::vector<AstNode *>::size_type i = 0; i != children.size(); i++)
	{
		delete children[i];
	}
}

void AstNode::addChild(AstNode *node)
{
	children.push_back(node);
}

const std::map<const char *, TokenType> Parser::keywords = createKeywordMap();
const std::map<const char *, TokenType> Parser::operators = createOperatorMap();

/* Map operator strings to their associated token type */
const std::map<const char *, TokenType> Parser::createKeywordMap()
{
	std::map<const char *, TokenType> tm;

	tm["("] = OpenBracket;
	tm[")"] = CloseBracket;

	return tm;
}

/* Map keyword strings to their associated token type */
const std::map<const char *, TokenType> Parser::createOperatorMap()
{
	std::map<const char *, TokenType> tm;

	tm["package"] = Package;
	tm["import"] = Import;

	return tm;
}

/* Parses the token at input as a keyword defined by str.
 * Returns the length of the token if parsing was successful, 0 otherwise.
 */
size_t Parser::parseKeyword(const char *input, const char *str)
{
	size_t i = 0;
	while (input[i] && str[i])
	{
		if (input[i] != str[i])
		{
			break;
		}
		i++;
	}
	if (i == strlen(str) && !isalpha(input[i]))
	{
		return i;
	}
	return 0;
}

/* Parses the token at input as an operator defined by str.
 * Returns the length of the token if parsing was successful, 0 otherwise.
 */
size_t Parser::parseOperator(const char *input, const char *str)
{
	size_t i = 0;
	while (input[i] && str[i])
	{
		if (input[i] != str[i])
		{
			break;
		}
		i++;
	}
	if (i == strlen(str))
	{
		return i;
	}
	return 0;
}

/* Parses the token at input as a string literal.
 * Returns the length of the token if parsing was successful, 0 otherwise.
 */
size_t Parser::parseStringLiteral(const char *input)
{
	size_t i = 0;
	if (input[i] == '"')
	{
		i++;
		while (input[i] && input[i] != '\n')
		{
			if (input[i] == '"')
			{
				if (input[i - 1] != '\\')
				{
					i++;
					break;
				}
			}
			i++;
		}
	}
	return i;
}

/* Parses the token at input as an identifier.
 * Returns the length of the token if parsing was successful, 0 otherwise.
 */
size_t Parser::parseIdentifier(const char *input)
{
	size_t i = 0;
	if (isalpha(input[i]))
	{
		i++;
		while (isalnum(input[i]))
		{
			i++;
		}
	}
	return i;
}

/* Returns 1 if the next token was parsed successfully, 0 otherwise */
int Parser::parseNextToken()
{
	size_t len;

	// skip whitespace
	while (isspace(*input))
	{
		if (*input == '\n')
		{
			// let the parser know we've reached a new line
			newLine(input);
		}
		input++;
	}

	// skip comments
	while (*input == '/')
	{
		switch (*(input + 1))
		{
			case '/':
				// single line comment
				input++;
				while (*input && *input != '\n')
				{
					input++;
				}
				if (*input == '\n')
				{
					// let the parser know we've reached a new line
					newLine(input);
				}
				break;
			case '*':
				// multi-line comment
				input++;
				while (*input)
				{
					if (*input == '\n')
					{
						// let the parser know we've reached a new line
						newLine(input);
					}

					if (*input == '*')
					{
						input++;
						if (*input == '/')
						{
							input++;
							break;
						}
					}

					input++;
				}
				break;
		}
	}

	len = 1;

	// record the line and column numbers of the token
	currentToken.length = len;
	currentToken.type = Undefined;
	currentToken.line = currentLine;
	currentToken.column = input - lines[currentLine];

	// check if at end of input
	switch (*input)
	{
		case '\0':
			currentToken.type = EndOfFile;
			break;
		case '"':
			len = parseStringLiteral(input);
			if (len)
			{
				currentToken.type = StringLiteral;
			}
			break;
		default:
			currentToken.type = Undefined;
	}

	// check if token value matches a keyword
	if (currentToken.type == Undefined)
	{
		for (iter = keywords.begin(); iter != keywords.end(); iter++)
		{
			len = parseKeyword(input, iter->first);
			if (len)
			{
				currentToken.type = iter->second;
				break;
			}
		}
	}

	// check if token value matches an operator
	if (currentToken.type == Undefined)
	{
		for (iter = operators.begin(); iter != operators.end(); iter++)
		{
			len = parseOperator(input, iter->first);
			if (len)
			{
				currentToken.type = iter->second;
				break;
			}
		}
	}

	// if token is still undefined, check if it could be an identifier
	if (currentToken.type == Undefined)
	{
		len = parseIdentifier(input);
		if (len)
		{
			currentToken.type = Identifier;
		}
	}

	// if token is no longer undefined type
	if (currentToken.type != Undefined)
	{
		// record the length of the token
		currentToken.length = len;

		// move the input pointer past the current token
		input += currentToken.length;

		return 1;
	}

	// token is still undefined, so we throw an exception
	throw ParserException(currentToken);
    return 0;
}

/* Called by the parser when it encounters '\n' during tokenisation
 *
 * parameters
 *     where: the address where '\n' was found
 */
void Parser::newLine(const char *where)
{
	lines.push_back(where + 1);
	currentLine++;
}

/* prints the token beginning at line:column to the specified output stream
 * if there are non-printable characters in the token, their hex value is printed
 */
void Parser::printToken(std::ostream &out, Token tok)
{
	const char *ch = lines[tok.line] + tok.column;
	const char *end = ch + tok.length;

	while (ch < end)
	{
		if (isprint(*ch))
		{
			out << *ch;
		}
		else
		{
			out << std::hex << *ch;
		}

		ch++;
	}
}

/* functions that represent our grammar productions */
AstNode * Parser::buildAst()
{
	AstNode *node = new AstNode(AstRoot);

	parseNextToken();
	node->addChild(packageStatement());
	node->addChild(importStatements());

	return node;
}

int Parser::printAst(AstNode *node, int indent)
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
		case AstUndefined:
			std::cout << "undefined";
			break;
		case AstStringLiteral:
			std::cout << "string literal";
			break;
		case AstIdentifier:
			std::cout << "identifier";
			break;
		case AstImportStatements:
			std::cout << "import statements";
			break;
		case AstImportItem:
			std::cout << "import item";
			break;
		case AstPackageStatement:
			std::cout << "package statement";
			break;
		case AstImport:
			std::cout << "import";
			break;
		case AstPackage:
			std::cout << "package";
			break;
		case AstRoot:
			std::cout << "root";
			break;
		default:
			std::cout << "undefined";
			break;
	}
	std::cout << std::endl;

	for(std::vector<int>::size_type i = 0; i != node->children.size(); i++)
	{
		printAst(node->children[i], indent + 1);
	}


	return 1;
}

AstNode * Parser::packageStatement()
{
	AstNode *pkgNode;
	AstNode *strLitNode;

	if (currentToken.type != Package)
	{
		throw ParserException(currentToken);
	}
	pkgNode = new AstNode(AstPackage);
	parseNextToken();
	if (currentToken.type != Identifier)
	{
		throw ParserException(currentToken);
	}
	strLitNode = new AstNode(AstStringLiteral);
	parseNextToken();

	AstNode *node = new AstNode(AstPackageStatement);
	node->addChild(pkgNode);
	node->addChild(strLitNode);

	return node;
}

AstNode * Parser::importStatements()
{
	AstNode *impStmtNode;
	impStmtNode = importStatement();
	if (currentToken.type != EndOfFile)
	{
		return importStatements();
	}
	return impStmtNode;
}

AstNode * Parser::importStatement()
{

	if (currentToken.type != Import)
	{
		throw ParserException(currentToken);
	}
	AstNode *impNodeA = new AstNode(AstImport);
	AstNode *impNodeB;
	parseNextToken();

	switch (currentToken.type)
	{
		case Identifier:
			// fall through
		case StringLiteral:
			impNodeB = imports();
			break;
		case OpenBracket:
			parseNextToken();
			impNodeB = imports();
			if (currentToken.type == CloseBracket)
			{
				parseNextToken();
				break;
			}
			// else fall through
		default:
			throw ParserException(currentToken);
	}

	AstNode *node = new AstNode(AstImportStatements);
	node->addChild(impNodeA);
	node->addChild(impNodeB);

	return node;
}

AstNode * Parser::imports()
{
	AstNode *impIdNode = NULL;
	AstNode *impPathNode;
	if (currentToken.type == Identifier)
	{
		impIdNode = new AstNode(AstIdentifier);
		parseNextToken();
	}

	if (currentToken.type != StringLiteral)
	{
    	throw ParserException(currentToken);
    }

    impPathNode = new AstNode(AstStringLiteral);

	parseNextToken();

	if (currentToken.type != EndOfFile)
	{
		return imports();
	}

	AstNode *node = new AstNode(AstImportItem);
	node->addChild(impIdNode);
	node->addChild(impPathNode);

	return node;
}


void Parser::parse(const char *str)
{
	// use this pointer to move through the input
	input = str;

	// record the start of the input string as the start of a new line
	lines.push_back(input);

	// set the current line of input that we are on (0 means line 1)
	currentLine = 0;

	ast = buildAst();
}

/* Returns 1 if the Ast was printed successfully, 0 otherwise */
int Parser::printAst()
{
	return printAst(ast, 0);
}

/* expects zero-based line and column numbers */
ParserException::ParserException(Token t): tok(t)
{
	snprintf(msg, sizeof(msg) / sizeof(msg[0]), "%zu:%zu: Unexpected token", tok.line + 1, tok.column + 1);
}

Token ParserException::getToken()
{
	return tok;
}

const char *ParserException::what() const throw()
{
	return msg;
}

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

	size_t sizeIncrement = PARSER_STDIN_BUF_LEN;

	if (!(array = (char *) malloc(sizeIncrement)))
	{
		std::cerr << "Not enough memory to read input" << std::endl;
		return NULL;
	}

	size = 0;
	while ((array[size++] = getchar()) != EOF)
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

void printUsage(std::ostream& outputStream, char *programName)
{
	outputStream << "Usage: " << programName << " [FILE]" << std::endl;
}

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		printUsage(std::cerr, argv[0]);
	}

	char *input;

	// check if input was piped into the program via stdin
	if (!isatty(STDIN_FILENO))
	{
		// check if the user also specified an input file
		if (argc == 2)
		{
			std::cerr << "Detected input from stdin, ignoring file: \"" << argv[1] << "\"" << std::endl;
		}

		// read from stdin
		input = readStdin();
	}
	else
	{
		// check if the user specified an input file
		if (argc == 2)
		{
			// read from file
			input = readFile(argv[1]);
		}
		else
		{
			printUsage(std::cerr, argv[0]);
			std::cerr << "No input from stdin, and no file specified, exiting..." << std::endl;
			return 1;
		}
	}

	if (input == NULL)
	{
		std::cerr << "Failed to retrieve input, exiting..." << std::endl;
		return 1;
	}

	Parser parser;
	try
	{
		parser.parse(input);
		parser.printAst();
		std::cout << "OK" << std::endl;
	}
	catch(ParserException& e)
	{
		std::cerr << e.what() << ": ";
		parser.printToken(std::cerr, e.getToken());
		std::cerr << std::endl;
	}

	// cast to void * to remove const
	free(input);
	return 0;
}
