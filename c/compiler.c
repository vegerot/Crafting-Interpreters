#include "compiler.h"
#include "scanner.h"
#include <stdio.h>

typedef struct {
	Scanner* scanner;
	Token previous;
	Token current;
	bool had_error;
	bool in_panic_mode;
} Parser;

static Parser parser; // NOLINT

static void initParser() {
	parser.had_error = false;
	parser.in_panic_mode = false;
}

static void errorAt(Token* token, char const* message) {
	fprintf(stderr, "file.lox:%d", token->line);

	if (token->type == TOKEN_EOF) {
		fprintf(stderr, ":end_of_file:");
	}

	fprintf(stderr, " error: %s\n", message);
	fprintf(stderr, "\t%d | %.*s\n", token->line, token->length, token->start);
}

static void errorAtCurrent(char const* message) {
	if (parser.in_panic_mode)
		return;
	errorAt(&parser.current, message);
	parser.had_error = true;
}

static void error(char const* message) {
	if (parser.in_panic_mode)
		return;
	errorAt(&parser.previous, message);
	parser.had_error = true;
}

void advance() {
	parser.previous = parser.current;

	while (1) {
		parser.current = scanToken(parser.scanner);
		if (parser.current.type != TOKEN_ERROR)
			break;

		errorAtCurrent(parser.current.start);
	}
}

static void consume_or_error(TokenType type, char const* message) {
	if (parser.current.type != type)
		errorAtCurrent(message);
	advance();
	return;
}

bool compile(char const* source, Chunk* chunk) {
	Scanner scanner;
	initScanner(&scanner, source);
	initParser();

	advance();
	expression();
	consume(TOKEN_EOF, "expected end of expression");
	return !parser.had_error;
}

void debug_scanner_(char const* source) {
	Scanner scanner;
	initScanner(&scanner, source);
	int line = -1;
	while (1) {
		Token token = scanToken(&scanner);
		if (scanner.line != line) {
			line = token.line;
			printf("%4d ", token.line);
		} else {
			printf("   | ");
		}
		printf("%8s, '%.*s'\n", tokenTypeToString_(token.type), token.length,
			   token.start);

		if (token.type == TOKEN_EOF)
			break;
	}
}
