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

typedef struct Token
{
	TokenType type;

	// length of the token in bytes
	size_t length;

	// line and column numbers, zero-based
	size_t line;
	size_t column;
} Token;
