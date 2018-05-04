
#include "parser.h"

#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>

class ASTNode
{
public:
   ASTNodeType type;
   char value[1024] = {0};
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
	Token curTok;
	size_t index;
	const char *input;

private:
	/* returns 0 if input up to whitespace is equal to str, 1 otherwise. */
	int equalsString(const char *input, size_t index, const char *str, size_t *length)
	{
		int i = 0;
		while (input[index + i] && str[i])
		{
			if (input[index + i] != str[i])
			{
				break;
			}
			i++;
		}
		if (i == strlen(str) && (isspace(input[index + i]) || !input[index + i]))
		{
			*length = i;
			return 0;
		}
		return 1;
	}

	/* returns 0 if input up to whitespace is a valid string literal, 1 otherwise */
	int isStringLiteral(const char *input, size_t index, size_t *length)
	{
		if (input[index] != '"')
		{
			return 1;
		}

		int i = 0;
		while (input[index + i])
		{
			if (input[index + i] == '"' && i > 0)
			{
				if (input[index + i - 1] != '\\')
				{
					i++;
					*length = i;
					return 0;
				}
			}
			i++;
		}
		return 1;
	}

	/* returns 0 if at least one alphanumeric character is found, 1 otherwise. */
	int isIdentifier(const char *input, size_t index, size_t *length)
	{
		if (!isalpha(input[index]))
		{
			return 1;
		}

		int i = 0;
		while (input[index + i])
		{
			if (!isalnum(input[index + i]))
			{
				break;
			}
			i++;
		}
		if (i > 0)
		{
			*length = i;
			return 0;
		}
		return 1;
	}

	/* parse the next token */
	void getNextToken()
	{
		while (isspace(input[index]))
		{
			index++;
		}

		memset(curTok.value, 0, sizeof(curTok.value));
		curTok.length = 0;
		curTok.type = Error;

		// check if at end of input
		if (input[index] == 0)
		{
			curTok.type = EndOfText;
			return;
		}

		switch (input[index])
		{
			case '(':
				curTok.type = OpenBracket;
				curTok.length = 1;
				break;
			case ')':
				curTok.type = CloseBracket;
				curTok.length = 1;
				break;
			case '"':
				if (isStringLiteral(input, index, &curTok.length) == 0)
				{
					curTok.type = StringLiteral;
				}
				break;
			case 'i':
				if (equalsString(input, index, (const char *) "import", &curTok.length) == 0)
				{
					curTok.type = Import;
				}
				break;
			case 'p':
				if (equalsString(input, index, (const char *) "package", &curTok.length) == 0)
				{
					curTok.type = Package;
				}
				break;
			default:
				if (isIdentifier(input, index, &curTok.length) == 0)
				{
					curTok.type = Identifier;
				}
		}

		if (curTok.type != Error)
		{
			strncpy(curTok.value, input + index, curTok.length);
			index += curTok.length;
		}
		else
		{
			std::stringstream sstr;
			sstr << "Unexpected token '" << input[index] << "' at position " << index;
			throw ParserException(sstr.str(), index);
        }
	}

	/* functions that represent our grammar productions */
	ASTNode* buildAST()
	{
		ASTNode* pkgStmtNode = packageStatement();
		ASTNode* impStmtNode = importStatements();

		return createNode(ASTRoot, pkgStmtNode, impStmtNode);
	}

	void printAST(ASTNode *node, int indent)
	{
		for (int i = 0; i < indent; i++)
		{
			printf("\t");
		}

		switch (node->type)
		{
			case ASTUndefined:
				printf("(undefined):");
				break;
			case ASTStringLiteral:
				printf("(string literal):");
				break;
			case ASTIdentifier:
				printf("(identifier):");
				break;
			case ASTImportStatements:
				printf("(import statements):");
				break;
			case ASTImportItem:
				printf("(import item):");
				break;
			case ASTPackageStatement:
				printf("(package statement):");
				break;
			case ASTImport:
				printf("(import):");
				break;
			case ASTPackage:
				printf("(package):");
				break;
			case ASTRoot:
				printf("(root):");
				break;
			default:
				printf("(undefined):");
				break;
		}
		printf("%s\n", node->value);


		if (node->left)
		{
			printAST(node->left, indent + 1);
		}

		if (node->right)
		{
			printAST(node->right, indent + 1);
		}
	}

	ASTNode* createNode(ASTNodeType type, ASTNode* left, ASTNode* right)
	{
		ASTNode* node = new ASTNode;
		node->type = type;
		node->left = left;
		node->right = right;

		return node;
	}

	ASTNode* createStrLitNode(char *value)
	{
		ASTNode* node = new ASTNode;
		node->type = ASTStringLiteral;
		strncpy(node->value, value, sizeof(node->value));

		return node;
	}

	ASTNode* createIdentifierNode(char *value)
	{
		ASTNode* node = new ASTNode;
		node->type = ASTIdentifier;
		strncpy(node->value, value, sizeof(node->value));
		return node;
	}

	ASTNode* createImportNode(char *value)
	{
		ASTNode* node = new ASTNode;
		node->type = ASTImport;
		strncpy(node->value, value, sizeof(node->value));
		return node;
	}

	ASTNode* createPackageNode(char *value)
	{
		ASTNode* node = new ASTNode;
		node->type = ASTPackage;
		strncpy(node->value, value, sizeof(node->value));
		return node;
	}

	ASTNode* packageStatement()
	{
		ASTNode* pkgNode;
		ASTNode* strLitNode;

		if (curTok.type != Package)
		{
			std::stringstream sstr;
            sstr << "Unexpected token '" << curTok.value << "' at position " << index;
            throw ParserException(sstr.str(), index);
		}
		pkgNode = createPackageNode(curTok.value);
		getNextToken();
		if (curTok.type != Identifier)
		{
			std::stringstream sstr;
            sstr << "Unexpected token '" << curTok.value << "' at position " << index;
            throw ParserException(sstr.str(), index);
		}
		strLitNode = createStrLitNode(curTok.value);
		getNextToken();

		return createNode(ASTPackageStatement, pkgNode, strLitNode);
	}

	ASTNode* importStatements()
	{
		ASTNode *impStmtNode;
		impStmtNode = importStatement();
		if (curTok.type != EndOfText)
		{
			return importStatements();
		}
		return impStmtNode;
	}

	ASTNode* importStatement()
	{

		if (curTok.type != Import)
		{
			std::stringstream sstr;
            sstr << "Unexpected token '" << curTok.value << "' at position " << index;
            throw ParserException(sstr.str(), index);
		}
		ASTNode *impNodeA = createImportNode(curTok.value);
		ASTNode *impNodeB;
		getNextToken();

		switch (curTok.type)
		{
			case Identifier:
				// fall through
			case StringLiteral:
				impNodeB = imports();
				break;
			case OpenBracket:
				getNextToken();
				impNodeB = imports();
				if (curTok.type == CloseBracket)
				{
					getNextToken();
					break;
				}
				// else fall through
			default:
				std::stringstream sstr;
            	sstr << "Unexpected token '" << curTok.value << "' at position " << index;
            	throw ParserException(sstr.str(), index);
		}

		return createNode(ASTImportStatements, impNodeA, impNodeB);
	}

	ASTNode* imports()
	{
		ASTNode *impIdNode = NULL;
		ASTNode *impPathNode;
		if (curTok.type == Identifier)
		{
			impIdNode = createIdentifierNode(curTok.value);
			getNextToken();
		}

		if (curTok.type != StringLiteral)
		{
			std::stringstream sstr;
        	sstr << "Unexpected token '" << curTok.value << "' at position " << index;
        	throw ParserException(sstr.str(), index);
        }

        impPathNode = createStrLitNode(curTok.value);

		getNextToken();

		if (curTok.type != EndOfText)
		{
			return imports();
		}

		return createNode(ASTImportItem, impIdNode, impPathNode);
	}

public:
	void parse(const char* str)
	{
		input = str;
		index = 0;

		getNextToken();
		printAST(buildAST(), 0);
	}
};

char *fileToCharArray(char *fname)
{
	FILE *fp;
	char *array;

	if (!(fp = fopen(fname, "rb")))
	{
		fprintf(stderr, "Couldn't open file: \"%s\"\n", fname);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (!(array = (char *) malloc(fsize + 1)))
	{
		fprintf(stderr, "Couldn't allocate memory to store the contents of file: \"%s\"\n", fname);
		fclose(fp);
		return NULL;
	}

	if (fread(array, sizeof(char), fsize, fp) != fsize)
	{
		fprintf(stderr, "Couldn't read entire file: \"%s\"\n", fname);
		fclose(fp);
		free(array);
		return NULL;
	}

	array[fsize] = 0;

	return array;
}

void printUsage(FILE *fp, char *progName)
{
	fprintf(fp, "Usage: %s [FILE]\n", progName);
}

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		printUsage(stderr, argv[0]);
	}

	char *input;

	if (!isatty(STDIN_FILENO))
	{
		if (argc == 2)
		{
			printf("Detected input from stdin; ignoring file: %s\n", argv[1]);
		}

		int size = PIPE_BUFFER_SIZE, i = 0;
		input = (char *) malloc(size);

		while ((input[i++] = getchar()) != EOF)
		{
			if (i == sizeof(input))
			{
				input = (char *) realloc(input, i + PIPE_BUFFER_SIZE);
			}
		}
		input[i - 1] = '\0';
	}
	else
	{
		if (argc == 2)
		{
			input = fileToCharArray(argv[1]);
			if (input == NULL)
			{
				fprintf(stderr, "Failed to retrieve input, exiting...\n");
				return 1;
			}
		}
		else
		{
			fprintf(stderr, "Error: Expected input from stdin, since no FILE was specified.\n");
			printUsage(stderr, argv[0]);
			return 1;
		}
	}

	Parser parser;
	try
	{
		parser.parse(input);
		std::cout << "OK" << std::endl;
	}
	catch(ParserException& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	free(input);
	return 0;
}
