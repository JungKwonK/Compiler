
#include "parser.hpp"

#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <iostream>

class ASTNode
{
public:
   ASTNodeType type;
   ASTNode* left;
   ASTNode* right;

   ASTNode()
   {
      type = ASTUndefined;
      left = NULL;
      right = NULL;
   }

   ~ASTNode()
   {
      delete left;
      delete right;
   }
};

class Parser
{
	ASTNode *ast = NULL;

	Token currentToken; // data about current token

	const char *start;	// start of input buffer
	const char *input;	// current position in input buffer

private:
	/* Parses the token at input as a keyword defined by str.
	 * Returns the length of the token if parsing was successful, 0 otherwise.
	 */
	size_t parseKeyword(const char *input, const char *str)
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

	/* Parses the token at input as a string literal.
	 * Returns the length of the token if parsing was successful, 0 otherwise.
	 */
	size_t parseStringLiteral(const char *input)
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
	size_t parseIdentifier(const char *input)
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
	int parseNextToken()
	{
		size_t len = 1;

		currentToken.type = Undefined;

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
		if (*input == '\0')
		{
			currentToken.type = EndOfFile;
		}

		// parse the token
		switch (*input)
		{
			case '(':
				currentToken.type = OpenBracket;
				break;
			case ')':
				currentToken.type = CloseBracket;
				break;
			case '"':
				len = parseStringLiteral(input);
				if (len)
				{
					currentToken.type = StringLiteral;
				}
				break;
			case 'i':
				len = parseKeyword(input, (const char *) "import");
				if (len)
				{
					currentToken.type = ImportKeyword;
				}
				break;
			case 'p':
				len = parseKeyword(input, (const char *) "package");
				if (len)
				{
					currentToken.type = PackageKeyword;
				}
				break;
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
	ASTNode* buildAST()
	{
		parseNextToken();

		ASTNode* pkgStmtNode = packageStatement();
		ASTNode* impStmtNode = importStatements();

		return createNode(ASTRoot, pkgStmtNode, impStmtNode);
	}

	int printAST(ASTNode *node, int indent)
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
			case ASTUndefined:
				std::cout << "undefined";
				break;
			case ASTStringLiteral:
				std::cout << "string literal";
				break;
			case ASTIdentifier:
				std::cout << "identifier";
				break;
			case ASTImportStatements:
				std::cout << "import statements";
				break;
			case ASTImportItem:
				std::cout << "import item";
				break;
			case ASTPackageStatement:
				std::cout << "package statement";
				break;
			case ASTImport:
				std::cout << "import";
				break;
			case ASTPackage:
				std::cout << "package";
				break;
			case ASTRoot:
				std::cout << "root";
				break;
			default:
				std::cout << "undefined";
				break;
		}
		std::cout << std::endl;

		if (node->left)
		{
			printAST(node->left, indent + 1);
		}

		if (node->right)
		{
			printAST(node->right, indent + 1);
		}

		return 1;
	}

	ASTNode* createNode(ASTNodeType type, ASTNode* left, ASTNode* right)
	{
		ASTNode* node = new ASTNode;
		node->type = type;
		node->left = left;
		node->right = right;

		return node;
	}

	ASTNode* packageStatement()
	{
		ASTNode* pkgNode;
		ASTNode* strLitNode;

		if (currentToken.type != PackageKeyword)
		{
			throw ParserException(*input, input - start);
		}
		pkgNode = createNode(ASTPackage, NULL, NULL);
		parseNextToken();
		if (currentToken.type != Identifier)
		{
			throw ParserException(*input, input - start);
		}
		strLitNode = createNode(ASTStringLiteral, NULL, NULL);
		parseNextToken();

		return createNode(ASTPackageStatement, pkgNode, strLitNode);
	}

	ASTNode* importStatements()
	{
		ASTNode *impStmtNode;
		impStmtNode = importStatement();
		if (currentToken.type != EndOfFile)
		{
			return importStatements();
		}
		return impStmtNode;
	}

	ASTNode* importStatement()
	{

		if (currentToken.type != ImportKeyword)
		{
			throw ParserException(*input, input - start);
		}
		ASTNode *impNodeA = createNode(ASTImport, NULL, NULL);
		ASTNode *impNodeB;
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

		return createNode(ASTImportStatements, impNodeA, impNodeB);
	}

	ASTNode* imports()
	{
		ASTNode *impIdNode = NULL;
		ASTNode *impPathNode;
		if (currentToken.type == Identifier)
		{
			impIdNode = createNode(ASTIdentifier, NULL, NULL);
			parseNextToken();
		}

		if (currentToken.type != StringLiteral)
		{
        	throw ParserException(*input, input - start);
        }

        impPathNode = createNode(ASTStringLiteral, NULL, NULL);

		parseNextToken();

		if (currentToken.type != EndOfFile)
		{
			return imports();
		}

		return createNode(ASTImportItem, impIdNode, impPathNode);
	}

public:
	void parse(const char* str)
	{
		// set this pointer to remember the start address of the input
		start = str;
		// use this pointer to move through the input
		input = start;

		ast = buildAST();
	}

	/* Returns 1 if the AST was printed successfully, 0 otherwise */
	int printAST()
	{
		return printAST(ast, 0);
	}
};

/* Returns a malloc'ed char array if fname was successfully read, NULL otherwise.
 * The caller is responsible for freeing the returned pointer */
char *readFile(char *fname)
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
char *readStdin()
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
		parser.printAST();
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
