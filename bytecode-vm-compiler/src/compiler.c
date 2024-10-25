#include <stdio.h>
#include <string.h>

#include "chunk.h"
#include "compiler.h"
#include "lox_assert.h"
#include "object.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

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
	/// prefix is a function that is called when the token is the first token in
	/// an expression.  It is used for unary operators and literals.
	void (*prefix)();
	ParseFn infix;
	Precedence precedence;
} ParseRule;

static Parser parser;					   // NOLINT
static Chunk* compilingChunk = (Chunk*)69; // NOLINT

static Chunk* currentChunk() { return compilingChunk; }

static void initParser(Scanner* scanner) {
	parser.had_error = false;
	parser.in_panic_mode = false;
	parser.scanner = scanner;
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

LoxString* fromCString(char const* cString, size_t length) {
	LoxString* str = malloc(sizeof(LoxString) + (length + 1) * sizeof(char));
	str->obj.type = OBJ_STRING;
	str->obj.next = currentChunk()->allocatorStart;

	currentChunk()->allocatorStart = (LoxObj*)str;

	strncpy(str->chars, cString, length);
	str->chars[length + 1] = '\0';

	str->length = length;
	return str;
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
		/* TODO: figure out how to unroll loop*/                               \
		for (size_t i = 0; i < sizeof(bytes) / sizeof(bytes[0]); ++i) {        \
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

static void EndCompiler() {
	EmitReturn();
#ifdef DEBUG_PRINT_CODE
	if (!parser.had_error) {
		printf("DISASSEMBLY START\n");
		dissasembleChunk(currentChunk(), "code");
		printf("DISASSEMBLY END\n");
	}
#endif
}
static void Expression();
static ParseRule const* GetRule(TokenType type);
static void ParsePrecedence(Precedence precedence);

/// Note[ConditionalOperator]
/// This does not implement the conditional operator, but just gives the general
/// shape of it.
/// We don't know how to do branches yet, so this function splits up the work we
/// _will_ do into three blocks:
/// 1. Initialize branching code
/// 2. Compile the true branch
/// 3. Compile the false branch
/// When branching is implemented we will replace the code in these blocks with
/// the relevant branching code.  It will probably end up bytecode identical
/// with an if/else expression.
static void Ternary() {
	TokenType operatorType = parser.previous.type;
	LOX_ASSERT_EQUALS(operatorType, TOKEN_QUESTION);
	ParseRule const* rule = GetRule(operatorType);
	LOX_ASSERT_EQUALS(rule->precedence, PREC_TERM)

	// 1. initialize branches
	// for now, just emit a placeholder instruction
	{
		EmitBytes(OP_CONDITIONAL_PLACEHOLDER);
	}

	// 2. compile true branch
	// for now, just write the bytecode for the true branch
	{
		ParsePrecedence(rule->precedence + 1);
	}

	consume_or_error(TOKEN_COLON,
					 "Missing ':' before false branch of ternary operator");

	// 3. compile false branch
	// for now, just write the bytecode for the false branch
	{
		ParsePrecedence(PREC_ASSIGNMENT);
	}
}

static void Binary() {
	TokenType operatorType = parser.previous.type;
	ParseRule const* rule = GetRule(operatorType);
	ParsePrecedence(rule->precedence + 1);

	switch (operatorType) {
	case TOKEN_EQUAL_EQUAL:
		EmitBytes(OP_EQUAL);
		break;
	case TOKEN_BANG_EQUAL:
		EmitBytes(OP_EQUAL, OP_NOT);
		break;
	case TOKEN_GREATER:
		EmitBytes(OP_GREATER);
		break;
	case TOKEN_GREATER_EQUAL:
		EmitBytes(OP_LESS, OP_NOT);
		break;
	case TOKEN_LESS:
		EmitBytes(OP_LESS);
		break;
	case TOKEN_LESS_EQUAL:
		EmitBytes(OP_GREATER, OP_NOT);
		break;
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
		LOX_UNREACHABLE("invalid binary operator");
		return;
	}
}

static void Literal() {
	// perfnote: we could use separate functions for each of these
	switch (parser.previous.type) {
	case TOKEN_FALSE:
		EmitByte(OP_FALSE);
		break;
	case TOKEN_TRUE:
		EmitByte(OP_TRUE);
		break;
	case TOKEN_NIL:
		EmitByte(OP_NIL);
		break;
	default:
		LOX_UNREACHABLE("invalid literal");
	}
}

static void Grouping() {
	Expression();
	consume_or_error(TOKEN_RIGHT_PAREN, "Expected ')' after expression");
}

static void Number() {
	LOX_ASSERT(parser.previous.type == TOKEN_NUMBER);
	Value value = {.type = VAL_NUMBER,
				   .as.number = strtod(parser.previous.start, NULL)};
	EmitConstant(value);
}

static void String() {
	LOX_ASSERT(parser.previous.type == TOKEN_STRING);

	// TODO: support string escape sequences like '\n'
	int strLength = parser.previous.length - 2; // subtract quotes

	// FIXME: This sucks.  We need to embed the string into the bytecode
	// directly instead of allocating on the heap because now the bytecode
	// isn't portable.
	LoxString* str = fromCString(parser.previous.start + 1, strLength);

	Value value = {.type = VAL_OBJ, .as.obj = (LoxObj*)str};

	EmitConstant(value);
}

static void Unary() {
	TokenType operatorType = parser.previous.type;

	// compile the operand
	ParsePrecedence(PREC_UNARY);

	switch (operatorType) {
	case TOKEN_BANG:
		EmitBytes(OP_NOT);
		break;
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
	[TOKEN_BANG] = {Unary, NULL, PREC_NONE},
	[TOKEN_BANG_EQUAL] = {NULL, Binary, PREC_EQUALITY},
	[TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
	[TOKEN_EQUAL_EQUAL] = {NULL, Binary, PREC_EQUALITY},
	[TOKEN_GREATER] = {NULL, Binary, PREC_COMPARISON},
	[TOKEN_GREATER_EQUAL] = {NULL, Binary, PREC_COMPARISON},
	[TOKEN_LESS] = {NULL, Binary, PREC_COMPARISON},
	[TOKEN_LESS_EQUAL] = {NULL, Binary, PREC_COMPARISON},
	[TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
	[TOKEN_STRING] = {String, NULL, PREC_NONE},
	[TOKEN_NUMBER] = {Number, NULL, PREC_NONE},
	[TOKEN_AND] = {NULL, NULL, PREC_NONE},
	[TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
	[TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
	[TOKEN_FALSE] = {Literal, NULL, PREC_NONE},
	[TOKEN_FOR] = {NULL, NULL, PREC_NONE},
	[TOKEN_FUN] = {NULL, NULL, PREC_NONE},
	[TOKEN_IF] = {NULL, NULL, PREC_NONE},
	[TOKEN_NIL] = {Literal, NULL, PREC_NONE},
	[TOKEN_OR] = {NULL, NULL, PREC_NONE},
	[TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
	[TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
	[TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
	[TOKEN_THIS] = {NULL, NULL, PREC_NONE},
	[TOKEN_TRUE] = {Literal, NULL, PREC_NONE},
	[TOKEN_VAR] = {NULL, NULL, PREC_NONE},
	[TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
	[TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
	[TOKEN_EOF] = {NULL, NULL, PREC_NONE},
	[TOKEN_QUESTION] = {NULL, Ternary, PREC_TERM},
	[TOKEN_COLON] = {NULL, NULL, PREC_NONE},
};

static void ParsePrecedence(Precedence precedence) {
	advance();
	ParseFn prefixRule = GetRule(parser.previous.type)->prefix;
	if (prefixRule == NULL) {
		errorAtCurrent("Expect expression");
		return;
	}

	prefixRule();

	while (precedence <= GetRule(parser.current.type)->precedence) {
		advance();
		ParseFn infixRule = GetRule(parser.previous.type)->infix;
		infixRule();
	}
}

static ParseRule const* GetRule(TokenType type) { return &rules[type]; }

static void Expression() { ParsePrecedence(PREC_ASSIGNMENT); }

void setCompilingChunk(Chunk* chunk) { compilingChunk = chunk; }

bool compile(char const* source, Chunk* chunk) {
	Scanner scanner;
	initScanner(&scanner, source);
	setCompilingChunk(chunk);
	initParser(&scanner);

	advance();
	Expression();
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
