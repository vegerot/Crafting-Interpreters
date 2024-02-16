#include "compiler.h"
#include "chunk.h"
#include "lox_assert.h"
#include "scanner.h"
#include <stdio.h>

typedef struct {
	Scanner* scanner;
	Token previous;
	Token current;
	bool had_error;
	bool in_panic_mode;
} Parser;

typedef enum {
	PREC_NONE = 0,
	PREC_ASSIGNMENT, // =
	PREC_OR,		 // or
	PREC_AND,		 // and
	PREC_EQUALITY,	 // == !=
	PREC_COMPARISON, // < > <= >=
	PREC_TERM,		 // + -
	PREC_FACTOR,	 // * /
	PREC_UNARY,		 // ! -
	PREC_CALL,		 // . ()
	PREC_PRIMARY,
} Precedence;

typedef void (*ParseFn)();

typedef struct {
	void (*prefix)();
	ParseFn infix;
	Precedence precedence;
} ParseRule;

static Parser parser;		  // NOLINT
static Chunk* compilingChunk; // NOLINT

static Chunk* currentChunk() { return compilingChunk; }

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

static void EmitByte(uint8_t byte) {
	writeChunk(currentChunk(), byte, parser.previous.line);
}

#define EmitBytes(...)                                                         \
	do {                                                                       \
		uint8_t bytes[] = {__VA_ARGS__};                                       \
		_Pragma("unroll") for (size_t i = 0;                                   \
							   i < sizeof(bytes) / sizeof(bytes[0]); ++i) {    \
			EmitByte(bytes[i]);                                                \
		}                                                                      \
	} while (0)

static void EmitReturn() { EmitByte(OP_RETURN); }

static uint8_t MakeConstant(Value value) {
	int constant = addConstant(currentChunk(), value);
	if (constant > UINT8_MAX) {
		error("Too many constants in chunk");
		return 0;
	}
	return (uint8_t)constant;
}

static void EmitConstant(Value value) {
	EmitBytes(OP_CONSTANT, MakeConstant(value));
}

static void EndCompiler() { EmitReturn(); }

static void Binary() {
	TokenType operatorType = parser.previous.type;
	ParseRule* rule = GetRule(operatorType);
	ParsePrecedence(rule->precedence + 1);

	switch (operatorType) {
	case TOKEN_PLUS:
		EmitBytes(OP_ADD);
		break;
	case TOKEN_MINUS:
		EmitBytes(OP_SUBTRACT);
		break;
	case TOKEN_STAR:
		EmitBytes(OP_MULTIPLY);
		break;
	case TOKEN_SLASH:
		EmitBytes(OP_DIVIDE);
		break;
	default:
		LOX_UNREACHABLE("unreachable");
		return;
	}
}

static void Grouping() {
	// expression();
	consume_or_error(TOKEN_RIGHT_PAREN, "Expected ')' after expression");
}

static void Number() {
	LOX_ASSERT(parser.previous.type == TOKEN_NUMBER);
	double value = strtod(parser.previous.start, NULL);
	EmitConstant(value);
}

static void Unary() {
	TokenType operatorType = parser.previous.type;

	// compile the operand
	ParsePrecedence(PREC_UNARY);
	// expression();

	switch (operatorType) {
	case TOKEN_MINUS:
		EmitBytes(OP_NEGATE);
		break;
	default:
		LOX_UNREACHABLE("unexpected operator");
		return;
	}
}

static ParseRule const rules[] = {
	[TOKEN_LEFT_PAREN] = {Grouping, NULL, PREC_NONE},
	[TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
	[TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
	[TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
	[TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
	[TOKEN_DOT] = {NULL, NULL, PREC_NONE},
	[TOKEN_MINUS] = {Unary, Binary, PREC_TERM},
	[TOKEN_PLUS] = {NULL, Binary, PREC_TERM},
	[TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
	[TOKEN_SLASH] = {NULL, Binary, PREC_FACTOR},
	[TOKEN_STAR] = {NULL, Binary, PREC_FACTOR},
	[TOKEN_BANG] = {NULL, NULL, PREC_NONE},
	[TOKEN_BANG_EQUAL] = {NULL, NULL, PREC_NONE},
	[TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
	[TOKEN_EQUAL_EQUAL] = {NULL, NULL, PREC_NONE},
	[TOKEN_GREATER] = {NULL, NULL, PREC_NONE},
	[TOKEN_GREATER_EQUAL] = {NULL, NULL, PREC_NONE},
	[TOKEN_LESS] = {NULL, NULL, PREC_NONE},
	[TOKEN_LESS_EQUAL] = {NULL, NULL, PREC_NONE},
	[TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
	[TOKEN_STRING] = {NULL, NULL, PREC_NONE},
	[TOKEN_NUMBER] = {Number, NULL, PREC_NONE},
	[TOKEN_AND] = {NULL, NULL, PREC_NONE},
	[TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
	[TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
	[TOKEN_FALSE] = {NULL, NULL, PREC_NONE},
	[TOKEN_FOR] = {NULL, NULL, PREC_NONE},
	[TOKEN_FUN] = {NULL, NULL, PREC_NONE},
	[TOKEN_IF] = {NULL, NULL, PREC_NONE},
	[TOKEN_NIL] = {NULL, NULL, PREC_NONE},
	[TOKEN_OR] = {NULL, NULL, PREC_NONE},
	[TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
	[TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
	[TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
	[TOKEN_THIS] = {NULL, NULL, PREC_NONE},
	[TOKEN_TRUE] = {NULL, NULL, PREC_NONE},
	[TOKEN_VAR] = {NULL, NULL, PREC_NONE},
	[TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
	[TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
	[TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

static void ParsePrecedence(Precedence precedence) {}

static ParseRule GetRule(TokenType type) { return rules[type]; }

static void ParseExpression() { ParsePrecedence(PREC_ASSIGNMENT); }

bool compile(char const* source, Chunk* chunk) {
	(void)chunk;
	Scanner scanner;
	initScanner(&scanner, source);
	compilingChunk = chunk;
	initParser();
	EmitBytes(OP_NEGATE, OP_CONSTANT);

	advance();
	// expression();
	consume_or_error(TOKEN_EOF, "expected end of expression");
	EndCompiler();
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
