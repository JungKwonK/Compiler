enum TokenType
{
	ImportStatement,
	PackageStatement,
	Import,
	Package,
	OpenBracket
	CloseBracket,
	Identifier,
	EndOfText,
	StringLiteral
}

typedef struct Token
{
	TokenType type;
	char value[1024];

	Token():type(Error), value(0)
	{}
} Token;

class Parser
{
	Token curTok;
	size_t index;
	const char *input;

	void skipWhitespace(char **input, int *index)
	{
		while (isSpace(*input[index]))
		{
			*index++;
		}
	}
	
	/* returns 0 if input until whitespace is equal to the token, 0 otherwise.
	 */
	int equalsToken(char **input, int index, char *token)
	{
		int i = 0;
		while (input[index + i] && token[i])
		{
			if (input[index] != importStr[i])
			{
				break;
			}
			i++;
		}
		if (i == strlen(token) && (isSpace(input[i]) || !input[i]))
		{
			return 0;
		}
		return 1;
	}
	
	int getStringLiteral(input, index, &value)
	{
		if (input[index] != '"')
		{
			return 1;
		}

		int i = 0;
		while (input[index + i] && token[i])
		{
			if (input[index] == '"' && i > 0)
			{
				i++;
				break;
			}
			i++;
		}
		if (isSpace(input[i]) || !input[i])
		{
			if (i <= sizeof(value))
			{
				if (strncpy(input, value, i))
				{
					return 0;
				}
			}
		}
		return 1;
	}
	
	int getIdentifier(input, index, &curTok.value)
	{
	
	}
	
	void getNextToken()
	{
		skipWhitespace(&input, &index);
	
		curTok.value = 0;
		curTok.symbol = 0;
	
		// check if at end of input
		if (input[index] == 0)
		{
			curTok.type = EndOfText;
			return;
		}

		curTok.type = Error;

		switch (input[index])
		{
			case '(': 
				curTok.type = OpenBracket;
				break;
			case ')':
				curTok.type = CloseBracket;
				break;
			case '"':
				if (isStringLiteral(input, index, &curTok.value))
				{
					curTok.type = Identifier;
				}
				break;
			case 'i':
				if (equalsToken(input, index, "import"))
				{
					curTok.type = Import;
				}
				break;
			case 'p':
				if (equalsToken(input, index, "package"))
				{
					curTok.type = Package;
				}
				break;
			default:
				if (isIdentifier(input, index, &curTok.value))
				{
					curTok.type = Identifier;
				}
		}
		
		if (curTok.type != Error)
		{
			curTok
		}
		
		
	}
}
