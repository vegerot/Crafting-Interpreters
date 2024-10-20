#pragma once

typedef struct {
	char const* start_of_lexeme;
	char const* current;
	int line;
} Scanner;

void initScanner(Scanner* scanner, char const* source);

// clang-format off
#define TOKEN_TYPES \
	/* make first token ERROR to detect null tokens */\
	X(TOKEN_ERROR) \
	/* Single-character tokens.*/ \
	X(TOKEN_LEFT_PAREN) X(TOKEN_RIGHT_PAREN) X(TOKEN_LEFT_BRACE) X(TOKEN_RIGHT_BRACE) \
	X(TOKEN_COMMA) X(TOKEN_DOT) X(TOKEN_MINUS) X(TOKEN_PLUS) X(TOKEN_SEMICOLON) \
	X(TOKEN_SLASH) X(TOKEN_STAR) X(TOKEN_QUESTION) X(TOKEN_COLON) \
	/* One or two character tokens.*/ \
	X(TOKEN_BANG) X(TOKEN_BANG_EQUAL) X(TOKEN_EQUAL) X(TOKEN_EQUAL_EQUAL) X(TOKEN_GREATER) \
	X(TOKEN_GREATER_EQUAL) X(TOKEN_LESS) X(TOKEN_LESS_EQUAL) \
	/* Literals. */ \
	X(TOKEN_IDENTIFIER) X(TOKEN_STRING) X(TOKEN_NUMBER) \
	/* Keywords. */ \
	X(TOKEN_AND) X(TOKEN_CLASS) X(TOKEN_ELSE) X(TOKEN_FALSE) X(TOKEN_FOR) X(TOKEN_FUN) \
	X(TOKEN_IF) X(TOKEN_NIL) X(TOKEN_OR) X(TOKEN_PRINT) X(TOKEN_RETURN) X(TOKEN_SUPER) \
	X(TOKEN_THIS) X(TOKEN_TRUE) X(TOKEN_VAR) X(TOKEN_WHILE) \
 \
	X(TOKEN_EOF)

// clang-format on

#define X(name) name,
typedef enum { TOKEN_TYPES } TokenType;
#undef X

typedef struct {
	TokenType type;
	char const* start;
	int line;
	int length;
	/// precondition: MUST CHECK `type` is `TOKEN_ERROR` before accessing
	char const* error_message;
	/// precondition: MUST CHECK `type` is `TOKEN_ERROR` before accessing
	int error_message_length;
} Token;

Token scanToken(Scanner* scanner);

char* tokenTypeToString_(TokenType type);
