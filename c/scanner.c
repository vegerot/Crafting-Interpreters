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

Token scanToken(Scanner* scanner) {
	scanner->start_of_lexeme = scanner->current;
	if (isAtEnd(scanner)) {
		return makeToken(scanner, TOKEN_EOF);
	}
	return errorToken(scanner, "Unexpected character",
					  strlen("Unexpected character"));
}

char* tokenTypeToString_(TokenType type) {
	LOX_ASSERT(0 && type && "unimplemented");
}
