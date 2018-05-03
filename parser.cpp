enum TokenType
{
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
	
	/* returns 0 if input until whitespace is equal to str, 0 otherwise.
	 */
	int equalsString(char *input, size_t index, char *str, size_t *length)
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
		if (i == strlen(str) && (isSpace(input[index + i]) || !input[index + i]))
		{
			*length = i;
			return 0;
		}
		return 1;
	}
	
	int isStringLiteral(char *input, size_t index, size_t *length)
	{
		if (input[index] != '"')
		{
			return 1;
		}

		int i = 0;
		while (input[index + i] && token[i])
		{
			if (input[index + i] == '"' && i > 0)
			{
				if (input[index + i - 1] != '\\')
				{
					i++;
					break;
				}
			}
			i++;
		}
		if (isSpace(input[index + i]) || !input[index + i])
		{
			*length = i;
			return 0;
		}
		return 1;
	}
	
	int isIdentifier(char *input, size_t index, size_t *length)
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
		if (i > 0 && isSpace(input[index + i]) || !input[index + i])
		{
			*length = i;
			return 0;
		}
		return 1;
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
				curTok.length = 1;
				break;
			case ')':
				curTok.type = CloseBracket;
				curTok.length = 1;
				break;
			case '"':
				if (isStringLiteral(input, index, &curTok.length))
				{
					curTok.type = StringLiteral;
				}
				break;
			case 'i':
				if (equalsString(input, index, "import", &curTok.length))
				{
					curTok.type = Import;
				}
				break;
			case 'p':
				if (equalsString(input, index, "package", &curTok.length))
				{
					curTok.type = Package;
				}
				break;
			default:
				if (isIdentifier(input, index, &curTok.length))
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
}
