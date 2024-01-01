#include <stdbool.h>
#include <string.h>

#include "lox_assert.h"
#include "scanner.h"
#include <stdbool.h>

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

Token errorToken(Scanner* scanner, char const* message, int message_length) {
	return (Token){
		.type = TOKEN_ERROR,
		.start = scanner->start_of_lexeme,
		.line = scanner->line,
		.length = (int)(scanner->current - scanner->start_of_lexeme),
		.error_message = message,
		.error_message_length = message_length,
	};
}

static char advance(Scanner* scanner) { return *scanner->current++; }

static void consumeUntilNewLine(Scanner* scanner) {
	while (!isAtEnd(scanner) && advance(scanner) != '\n') {
	}
	++scanner->line;
}

static char peek(Scanner* scanner) { return *scanner->current; }
static char peekNext(Scanner* scanner) {
	if (isAtEnd(scanner))
		return '\0';
	return scanner->current[1];
}

static Token scanString(Scanner* s) {
	LOX_ASSERT(s->current[-1] == '"');
	while (!isAtEnd(s) && *s->current++ != '"') {
		if (peek(s) == '\n')
			++s->line;
	};
	if (isAtEnd(s)) {
		return errorToken(s, "Unterminated string",
						  strlen("Unterminated string"));
	}
	return makeToken(s, TOKEN_STRING);
}

static bool ifMatchConsume(Scanner* scanner, char c) {
	if (peek(scanner) == c) {
		advance(scanner);
		return true;
	}
	return false;
}

static void skipWhitespaceAndComments(Scanner* scanner) {
	while (1) {
		char t = peek(scanner);
		switch (t) {
		case ' ':
		case '\r':
		case '\t':
			advance(scanner);
			break;
		case '\n':
			++scanner->line;
			advance(scanner);
			break;
		case '/':
			if (peekNext(scanner) == '/') {
				consumeUntilNewLine(scanner);
			}
			return;
		default:
			return;
		}
	}
}

Token scanToken(Scanner* scanner) {
	skipWhitespaceAndComments(scanner);
	scanner->start_of_lexeme = scanner->current;
	if (isAtEnd(scanner)) {
		return makeToken(scanner, TOKEN_EOF);
	}

	char c = advance(scanner);
	switch (c) {
	// simple tokens
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
	// one lookahead symbols
	case '=':
		return makeToken(scanner, ifMatchConsume(scanner, '=')
									  ? TOKEN_EQUAL_EQUAL
									  : TOKEN_EQUAL);
	case '!':
		return makeToken(scanner, ifMatchConsume(scanner, '=')
									  ? TOKEN_BANG_EQUAL
									  : TOKEN_BANG);
	case '<':
		return makeToken(scanner, ifMatchConsume(scanner, '=')
									  ? TOKEN_LESS_EQUAL
									  : TOKEN_LESS);
	case '>':
		return makeToken(scanner, ifMatchConsume(scanner, '=')
									  ? TOKEN_GREATER_EQUAL
									  : TOKEN_GREATER);
	case '"':
		return scanString(scanner);

	default:
		return errorToken(scanner, "Unexpected character",
						  strlen("Unexpected character"));
	}
}

char* tokenTypeToString_(TokenType type) {
	switch (type) {
		// clang-format off
#define X(name) case name: return #name;
		TOKEN_TYPES
#undef X
		//clang-format on
	}
	__builtin_unreachable();
}
