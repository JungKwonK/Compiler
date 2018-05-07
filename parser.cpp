
#include "parser.hpp"

#include <unistd.h>

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

const std::map<const char*, TokenType> Parser::keywords = createKeywordMap();
const std::map<const char*, TokenType> Parser::operators = createOperatorMap();

/* Map operator strings to their associated token type */
const std::map<const char*, TokenType> Parser::createKeywordMap()
{
  std::map<const char*, TokenType> tm;
  tm["("] = OpenBracket;
  tm[")"] = CloseBracket;
  return tm;
}

/* Map keyword strings to their associated token type */
const std::map<const char*, TokenType> Parser::createOperatorMap()
{
  std::map<const char*, TokenType> tm;
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
		while (input[i])
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
	size_t len = 1;

	// skip whitespace
	while (isspace(*input))
	{
		input++;
	}

	// skip comments
	if (*input == '/')
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
				break;
			case '*':
				// multi-line comment
				input++;
				while (*input)
				{
					if (*input++ == '*')
					{
						if (*input++ == '/')
						{
							break;
						}
					}
				}
				break;
		}
	}

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

	// if still undefined token, check if it could be an identifier
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
		// record the offset of the token from the beginning of the input buffer
		currentToken.offset = input - start;

		// move the input pointer past the current token
		input += currentToken.length;

		return 1;
	}

	// token could not be parsed. throw exception
	throw ParserException(*input, input - start);
    return 0;
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
		throw ParserException(*input, input - start);
	}
	pkgNode = new AstNode(AstPackage);
	parseNextToken();
	if (currentToken.type != Identifier)
	{
		throw ParserException(*input, input - start);
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
		throw ParserException(*input, input - start);
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
			throw ParserException(*input, input - start);
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
    	throw ParserException(*input, input - start);
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
	// set this pointer to remember the start address of the input
	start = str;
	// use this pointer to move through the input
	input = start;

	ast = buildAst();
}

/* Returns 1 if the Ast was printed successfully, 0 otherwise */
int Parser::printAst()
{
	return printAst(ast, 0);
}

/* Returns a malloc'ed char array if fname was successfully read, NULL otherwise.
 * The caller is responsible for freeing the returned pointer */
char * readFile(char *fname)
{
	char *array;

	size_t size;

	FILE *fp;

	if (!(fp = fopen(fname, "rb")))
	{
		std::cerr << "Couldn't open file: \"" << fname << "\"" << std::endl;
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (!(array = (char *) malloc(size + 1)))
	{
		std::cerr << "Not enough memory to read file: \"" << fname << "\"" << std::endl;
		fclose(fp);
		return NULL;
	}

	if (fread(array, sizeof(char), size, fp) != size)
	{
		std::cerr << "Couldn't read entire file: \"" << fname << "\"" << std::endl;
		fclose(fp);
		free(array);
		return NULL;
	}

	fclose(fp);

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

	array = (char *) malloc(sizeIncrement);

	size = 0;
	while ((array[size++] = getchar()) != EOF)
	{
		if (size == sizeof(array))
		{
			// allocate more memory every time the buffer is full
			array = (char *) realloc(array, size + sizeIncrement);
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
	catch(ParserException& ex)
	{
		std::cout << "Parser Exception: " << ex.what() << std::endl;
	}

	// cast to void * to remove const
	free(input);
	return 0;
}
