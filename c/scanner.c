#include <stdbool.h>
#include <string.h>

#include "lox_assert.h"
#include "scanner.h"

void initScanner(Scanner* scanner, char const* source) {
	scanner->start_of_lexeme = scanner->current = source;
	scanner->line = 1;
}

bool isAtEnd(Scanner* scanner) { return (*scanner->current == '\0'); }

Token makeToken(Scanner* scanner, TokenType type) {
	return (Token){
		.type = type,
		.start = scanner->start_of_lexeme,
		.length = (int)(scanner->current - scanner->start_of_lexeme),
		.line = scanner->line,
	};
}

Token errorToken(Scanner* scanner, char const* message, int length) {
	return (Token){
		.type = TOKEN_ERROR,
		.start = scanner->start_of_lexeme,
		.line = scanner->line,
		.length = length,
		.error_message = message,
	};
}

static char advance(Scanner* scanner) { return *scanner->current++; }

static char peek(Scanner* scanner) { return *scanner->current; }

static void skipWhitespace(Scanner* scanner) {
	while (1) {
		char t = peek(scanner);
		switch (t) {
		case ' ':
		case '\r':
		case '\t':
			advance(scanner);
			break;
		case '\n':
			advance(scanner);
			++scanner->line;
			break;
		default:
			return;
		}
	}
}

Token scanToken(Scanner* scanner) {
	skipWhitespace(scanner);
	scanner->start_of_lexeme = scanner->current;
	if (isAtEnd(scanner)) {
		return makeToken(scanner, TOKEN_EOF);
	}

	char c = advance(scanner);
	switch (c) {
	case '(':
		return makeToken(scanner, TOKEN_LEFT_PAREN);
	case ')':
		return makeToken(scanner, TOKEN_RIGHT_PAREN);
	case '{':
		return makeToken(scanner, TOKEN_LEFT_BRACE);
	case '}':
		return makeToken(scanner, TOKEN_RIGHT_BRACE);
	case ';':
		return makeToken(scanner, TOKEN_SEMICOLON);
	case ',':
		return makeToken(scanner, TOKEN_COMMA);
	case '.':
		return makeToken(scanner, TOKEN_DOT);
	case '+':
		return makeToken(scanner, TOKEN_PLUS);
	case '/':
		return makeToken(scanner, TOKEN_SLASH);
	case '*':
		return makeToken(scanner, TOKEN_STAR);
	}

	return errorToken(scanner, "Unexpected character",
					  strlen("Unexpected character"));
}

char* tokenTypeToString_(TokenType type) {
	switch (type) {
	#define X(name) case name: return #name;
	TOKEN_TYPES
	#undef X
	}
}
