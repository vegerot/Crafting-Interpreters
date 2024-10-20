#include "../src/chunk.h"
#include "../src/compiler.h"
#include "../src/debug.h"
#include "../src/lox_assert.h"
#include <stdio.h>
#include <string.h>

// STRING start
typedef struct {
	char* data;
	size_t len;
	size_t cap;
} string;

void appendToString(string* this, char const* that, size_t that_len) {
	if (this->data == NULL || this->cap == 0) {
		// initialize string
		this->cap = that_len * 2;
		this->data = malloc(this->cap * sizeof(char));
		this->len = that_len;
		strncpy(this->data, that, that_len);
		this->data[this->len] = '\0';
		return;
	}

	size_t remaining_space = this->cap - this->len;
	// printf("remaining_space: %lu\nthat_len: %lu\n", remaining_space,
	// that_len);

	if (remaining_space <= that_len) {
		this->cap = (this->cap + that_len) * 2;
		this->data = realloc( // NOLINT(bugprone-suspicious-realloc-usage)
			this->data, this->cap);
	}

	strncat(this->data, that, strlen(that));
	LOX_ASSERT_EQUALS(strlen(this->data),
					  that_len + this->len); // string accidentally truncated
	this->len += that_len;
	this->data[this->len] = '\0';
}

void freeString(string* this) { free(this->data); }

void initString(string* this) {
	this->len = 0;
	this->cap = 0;
	this->data = NULL;
}

void testNewString(void) {
	string s;
	initString(&s);
	appendToString(&s, "hello", 5);

	LOX_ASSERT_EQUALS(s.len, 5);
	LOX_ASSERT(s.cap >= s.len);
	// LOX_ASSERT_STRING_EQUALS(s.data, "hello")
	LOX_ASSERT(strcmp(s.data, "hello") == 0);
	freeString(&s);
}

void testGrowString(void) {
	string s;
	initString(&s);
	appendToString(&s, "the quick", 9);
	appendToString(&s, " brown fox jumps over the lazy dog", 34);

	LOX_ASSERT_EQUALS(s.len, 43);
	LOX_ASSERT(s.cap >= s.len);
	LOX_ASSERT(strcmp(s.data, "the quick brown fox jumps over the lazy dog") ==
			   0);
	freeString(&s);
}

void testStringAppend(void) {
	string s1 = {
		.data = NULL,
		.len = 0,
		.cap = 15,
	};

	appendToString(&s1, "hello", strlen("hello"));
	appendToString(&s1, " world", strlen(" world"));

	LOX_ASSERT_EQUALS((int)s1.len, 11);
	LOX_ASSERT(strcmp(s1.data, "hello world") == 0 && s1.data);
	freeString(&s1);
}

// STRING end

// DISASSEMBLER start

static void appendObjectToString(char* buf, size_t maxsize, Value value) {
	switch (OBJ_TYPE(value)) {
	case OBJ_STRING:
		snprintf(buf, maxsize, "%.*s", AS_STRING(value)->length,
				 AS_CSTRING(value));
		break;
	default:
		fprintf(stderr, "Cannot print object type %d\n", OBJ_TYPE(value));
		LOX_UNREACHABLE("cannot print this object type");
	}
}

static void appendValueToString(char* buf, size_t maxsize, Value value) {
	switch (value.type) {
	case VAL_NUMBER: {
		snprintf(buf, maxsize, "%.1f", value.as.number);
		break;
	}
	case VAL_BOOL: {
		snprintf(buf, maxsize, "%s", AS_BOOL(value) ? "true" : "false");
		break;
	}
	case VAL_NIL: {
		snprintf(buf, maxsize, "NIL");
		break;
	}
	case VAL_OBJ: {
		appendObjectToString(buf, maxsize, value);
		break;
	}
	default: {
		LOX_UNREACHABLE("cannot print unknown value type");
	}
	}
}

static int simpleInstruction(string* out, char const* name, int offset) {
	appendToString(out, name, strlen(name));
	appendToString(out, ";", 1);
	return offset + 1;
}

static int constantInstruction(string* out, char const* name, Chunk* chunk,
							   int offset) {
	uint8_t constantIndex = chunk->code[offset + 1];
	appendToString(out, name, strlen(name));
	char constantAsStr[40] = " ";
	appendValueToString(constantAsStr + 1, sizeof(constantAsStr),
						chunk->constants.values[constantIndex]);
	appendToString(out, constantAsStr, strlen(constantAsStr));
	appendToString(out, ";", 1);
	return offset + 2;
}

static int writeDisassembledInstruction(string* out, Chunk* chunk, int offset) {
	uint8_t instruction = chunk->code[offset];

	switch (instruction) {
	case OP_CONSTANT:
		return constantInstruction(out, "CONST", chunk, offset);
	case OP_TRUE:
		return simpleInstruction(out, "TRUE", offset);
	case OP_EQUAL:
		return simpleInstruction(out, "EQUAL", offset);
	case OP_GREATER:
		return simpleInstruction(out, "GREATER", offset);
	case OP_LESS:
		return simpleInstruction(out, "LESS", offset);
	case OP_ADD:
		return simpleInstruction(out, "ADD", offset);
	case OP_SUBTRACT:
		return simpleInstruction(out, "SUB", offset);
	case OP_MULTIPLY:
		return simpleInstruction(out, "MULT", offset);
	case OP_DIVIDE:
		return simpleInstruction(out, "DIV", offset);
	case OP_NOT:
		return simpleInstruction(out, "NOT", offset);
	case OP_NEGATE:
		return simpleInstruction(out, "NEG", offset);
	case OP_RETURN:
		return simpleInstruction(out, "RET", offset);
	case OP_CONDITIONAL_PLACEHOLDER:
		return simpleInstruction(out, "TERNARYPLACEHOLDER", offset);
	default:
		fflush(stdout);
		fprintf(stderr, "Unknown opcode 0x%x\n", instruction);
		LOX_ASSERT(false);
	}
}

static void writeDisassembledChunk(string* out, Chunk* chunk) {
	LOX_ASSERT(chunk->count > 0);
	for (int offset = 0; offset < chunk->count;) {
		offset = writeDisassembledInstruction(out, chunk, offset);
	}
	return;
}

void testDisassembleSimple(void) {
	Chunk chunk;
	initChunk(&chunk);

	// 1
	{
		int constant = addConstant(&chunk, NUMBER_VAL(1));
		writeChunk(&chunk, OP_CONSTANT, 2);
		writeChunk(&chunk, constant, 2);
	}

	char* want = "CONST 1.0;";
	string out;
	initString(&out);
	writeDisassembledChunk(&out, &chunk);
	char* got = out.data;

	LOX_ASSERT_STRING_EQUALS(got, want);
	freeString(&out);
}

void testDisassemble(void) {
	Chunk chunk;
	initChunk(&chunk);

	// 1*2 + 3
	{
		int constant = addConstant(&chunk, NUMBER_VAL(1));
		writeChunk(&chunk, OP_CONSTANT, 2);
		writeChunk(&chunk, constant, 2);

		constant = addConstant(&chunk, NUMBER_VAL(2));
		writeChunk(&chunk, OP_CONSTANT, 2);
		writeChunk(&chunk, constant, 2);

		writeChunk(&chunk, OP_MULTIPLY, 2);

		constant = addConstant(&chunk, NUMBER_VAL(300));
		writeChunk(&chunk, OP_CONSTANT, 2);
		writeChunk(&chunk, constant, 2);

		writeChunk(&chunk, OP_ADD, 2);

		writeChunkNoLine(&chunk, OP_RETURN);
	}

	char* want = "CONST 1.0;CONST 2.0;MULT;CONST 300.0;ADD;RET;";
	string out;
	initString(&out);
	writeDisassembledChunk(&out, &chunk);
	char* got = out.data;

	// TODO: instead of `strcmp`, walk through both strings for better
	// failure messages
	LOX_ASSERT(strcmp(got, want) == 0);
	freeString(&out);
}

void testDisassembleString() {
	Chunk chunk;
	initChunk(&chunk);

	// 1*2 + 3
	{
		int constant = addConstant(
			&chunk, (Value){.type = VAL_OBJ,
							.as.obj = (LoxObj*)fromCString("hello", 5)});
		writeChunk(&chunk, OP_CONSTANT, 2);
		writeChunk(&chunk, constant, 2);

		writeChunkNoLine(&chunk, OP_RETURN);
	}

	char* want = "CONST hello;RET;";
	string out;
	initString(&out);
	writeDisassembledChunk(&out, &chunk);
	char* got = out.data;

	LOX_ASSERT_STRING_EQUALS(got, want);
	freeString(&out);
}

// DISASSEMBLER end

void testCompile1plus1(void) {
	Chunk chunk;
	initChunk(&chunk);

	compile("1.0000+2.0", &chunk);

	char* want = "CONST 1.0;CONST 2.0;ADD;RET;";

	string out;
	initString(&out);
	writeDisassembledChunk(&out, &chunk);
	char* got = out.data;

	LOX_ASSERT(strcmp(got, want) == 0);
	freeString(&out);
}

void testCompileTernarySimple(void) {
	Chunk chunk;
	initChunk(&chunk);

	compile("0 ? 1 : 2", &chunk);

	char* want = "CONST 0.0;TERNARYPLACEHOLDER;CONST 1.0;CONST 2.0;RET;";

	string out;
	initString(&out);
	writeDisassembledChunk(&out, &chunk);
	char* got = out.data;

	LOX_ASSERT(strcmp(got, want) == 0);
	freeString(&out);
}

void testCompileTernaryComplex(void) {
	Chunk chunk;
	initChunk(&chunk);

	compile("0-0 ? (1+2) : 3*3", &chunk);

	char* want =
		"CONST 0.0;CONST 0.0;SUB;TERNARYPLACEHOLDER;CONST 1.0;CONST 2.0;ADD;"
		"CONST 3.0;CONST 3.0;MULT;RET;";

	string out;
	initString(&out);
	writeDisassembledChunk(&out, &chunk);
	char* got = out.data;

	LOX_ASSERT(strcmp(got, want) == 0);
	freeString(&out);
}

void testCompileNot(void) {
	Chunk chunk;
	initChunk(&chunk);

	compile("!true", &chunk);

	char* want = "TRUE;NOT;RET;";

	string out;
	initString(&out);
	writeDisassembledChunk(&out, &chunk);
	char* got = out.data;

	LOX_ASSERT(strcmp(got, want) == 0);
	freeString(&out);
}

void testCompileComparison(void) {
	Chunk chunk;

	string out;
	initChunk(&chunk);
	{
		bool success = compile("(true == true)", &chunk);
		LOX_ASSERT(success);

		char* want = "TRUE;TRUE;EQUAL;RET;";

		initString(&out);
		writeDisassembledChunk(&out, &chunk);
		char* got = out.data;

		LOX_ASSERT(strcmp(got, want) == 0);
	}
	freeString(&out);

	initChunk(&chunk);
	{
		bool success = compile("(true != true)", &chunk);
		LOX_ASSERT(success);

		char* want = "TRUE;TRUE;EQUAL;NOT;RET;";

		initString(&out);
		writeDisassembledChunk(&out, &chunk);
		char* got = out.data;

		LOX_ASSERT(strcmp(got, want) == 0);
	}
	freeString(&out);

	initChunk(&chunk);
	{
		bool s = compile("1 > 2", &chunk);
		LOX_ASSERT(s);

		char* want = "CONST 1.0;CONST 2.0;GREATER;RET;";

		initString(&out);
		writeDisassembledChunk(&out, &chunk);
		char* got = out.data;

		LOX_ASSERT(strcmp(got, want) == 0);
	}
	freeString(&out);

	initChunk(&chunk);
	{
		bool s = compile("3<4", &chunk);
		LOX_ASSERT(s);

		char* want = "CONST 3.0;CONST 4.0;LESS;RET;";

		initString(&out);
		writeDisassembledChunk(&out, &chunk);
		char* got = out.data;

		LOX_ASSERT(strcmp(got, want) == 0);
	}
	freeString(&out);

	initChunk(&chunk);
	{
		bool success = compile("5>=6", &chunk);
		LOX_ASSERT(success);

		char* want = "CONST 5.0;CONST 6.0;LESS;NOT;RET;";

		initString(&out);
		writeDisassembledChunk(&out, &chunk);
		char* got = out.data;

		LOX_ASSERT(strcmp(got, want) == 0);
	}
	freeString(&out);

	initChunk(&chunk);
	{
		bool s = compile("7<=8", &chunk);
		LOX_ASSERT(s);

		char* want = "CONST 7.0;CONST 8.0;GREATER;NOT;RET;";

		initString(&out);
		writeDisassembledChunk(&out, &chunk);
		char* got = out.data;

		LOX_ASSERT(strcmp(got, want) == 0);
	}
	freeString(&out);
}

void testStringSimple(void) {
	Chunk chunk;
	initChunk(&chunk);

	compile("\"string\"\n", &chunk);

	char* want = "CONST string;RET;";

	string out;
	initString(&out);
	writeDisassembledChunk(&out, &chunk);
	char* got = out.data;

	LOX_ASSERT_STRING_EQUALS(got, want);
	freeString(&out);
}

void testStringAdd() {
	Chunk chunk;
	initChunk(&chunk);

	compile("\"string\" + \"append\"", &chunk);

	char* want = "CONST string;CONST append;ADD;RET;";

	string out;
	initString(&out);
	writeDisassembledChunk(&out, &chunk);
	char* got = out.data;

	LOX_ASSERT_STRING_EQUALS(got, want);
	freeString(&out);
}

int main() {
	// testing testing tests
	{
		testNewString();
		testStringAppend();
		testGrowString();
		testDisassembleSimple();
		testDisassemble();
		testDisassembleString();
	}

	// actual compiler tests

	{
		testCompile1plus1();
		testCompileTernarySimple();
		testCompileTernaryComplex();
		testCompileNot();
		testCompileComparison();
		testStringSimple();
		testStringAdd();
	}
}
