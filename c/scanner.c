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

static bool isAlpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
static bool isDigit(char c) { return c >= '0' && c <= '9'; }

static Token scanNumber(Scanner* s) {
	LOX_ASSERT(isDigit(s->current[-1]));
	while (isDigit(peek(s))) {
		++s->current;
	}
	if (peek(s) == '.') {
		do {
			++s->current;
		} while (isDigit(peek(s)));
	}
	return makeToken(s, TOKEN_NUMBER);
}

TokenType checkKeyword(char const* start_of_lexeme, long length_of_lexeme,
					   char const* rest_of_kw, int length_of_chars_already_read,
					   int length_of_kw, TokenType type) {
	bool lengths_are_equal = length_of_lexeme == length_of_kw;
	if (!lengths_are_equal)
		return TOKEN_IDENTIFIER;
	char const* rest_of_lexeme = start_of_lexeme + length_of_chars_already_read;
	bool is_keyword = memcmp(rest_of_lexeme, rest_of_kw,
							 length_of_kw - length_of_chars_already_read) == 0;
	if (!is_keyword)
		return TOKEN_IDENTIFIER;
	return type;
}

TokenType identifierOrKeywordType(Scanner* s) {
	// FIXME: we compute this difference here and in `makeToken`
	long length_of_maybe_keyword = s->current - s->start_of_lexeme;
	switch (*s->start_of_lexeme) {
	case 'a':
		return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword, "nd",
							1, 3, TOKEN_AND);
	case 'c':
		return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword, "lass",
							1, 5, TOKEN_CLASS);
	case 'e':
		return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword, "lse",
							1, 4, TOKEN_ELSE);
	case 'f':
		switch (s->start_of_lexeme[1]) {
		case 'a':
			return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword,
								"lse", 2, 5, TOKEN_FALSE);
		case 'o':
			return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword,
								"r", 2, 3, TOKEN_FOR);
		case 'u':
			return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword,
								"n", 2, 3, TOKEN_FUN);
		default:
			return TOKEN_IDENTIFIER;
		}
	case 'i':
		return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword, "f", 1,
							2, TOKEN_IF);
	case 'n':
		return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword, "il",
							1, 3, TOKEN_NIL);
	case 'o':
		return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword, "r", 1,
							2, TOKEN_OR);
	case 'p':
		return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword, "rint",
							1, 5, TOKEN_PRINT);
	case 'r':
		return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword,
							"eturn", 1, 6, TOKEN_RETURN);
	case 's':
		return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword, "uper",
							1, 5, TOKEN_SUPER);
	case 't':
		switch (s->start_of_lexeme[1]) {
		case 'h':
			return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword,
								"is", 2, 4, TOKEN_THIS);
		case 'r':
			return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword,
								"ue", 2, 4, TOKEN_TRUE);
		default:
			return TOKEN_IDENTIFIER;
		}
	case 'v':
		return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword, "ar",
							1, 3, TOKEN_VAR);
	case 'w':
		return checkKeyword(s->start_of_lexeme, length_of_maybe_keyword, "hile",
							1, 5, TOKEN_WHILE);
	default:
		return TOKEN_IDENTIFIER;
	}
}

static Token scanIdentifierOrKeyword(Scanner* s) {
	while (isAlpha(peek(s)) || isDigit(peek(s))) {
		++s->current;
	}
	return makeToken(s, identifierOrKeywordType(s));
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

	if (isAlpha(c))
		return scanIdentifierOrKeyword(scanner);

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
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return scanNumber(scanner);
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
