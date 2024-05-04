#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/chunk.h"
#include "../src/debug.h"

char* hello(char const* name, int nameLen) {
	char* format = "Hello, %s!\n";
	// larger than required because of format specifiers
	int lengthOfTarget = (int)strlen(format) + nameLen;
	char* ret = malloc(sizeof(char) * lengthOfTarget);
	snprintf(ret, lengthOfTarget, format, name);
	return ret;
}

void shouldPrintHelloMax(void) {
	char* hiMax = hello("Max", 3);
	assert(strcmp(hiMax, "Hello, Max!\n") == 0);
	free(hiMax);
}

void testConstant(void) {
	Chunk chunk;
	initChunk(&chunk);
	assert(chunk.constants.capacity == 0);
	assert(chunk.constants.count == 0);
	addConstant(&chunk, 69.0);
	assert(chunk.constants.capacity == 8);
	assert(chunk.constants.count == 1);
	addConstant(&chunk, 420);
	addConstant(&chunk, 69.0);
	addConstant(&chunk, 420);
	addConstant(&chunk, 69.0);
	addConstant(&chunk, 420);
	addConstant(&chunk, 69.0);
	addConstant(&chunk, 420);
	assert(chunk.constants.capacity == 8);
	assert(chunk.constants.count == 8);
	addConstant(&chunk, 69.0);
	assert(chunk.constants.capacity == 0x10 && "grows when count > cap");
	assert(chunk.constants.count == 9);
	addConstant(&chunk, 420);
	addConstant(&chunk, 69.0);
	addConstant(&chunk, 420);
	addConstant(&chunk, 69.0);
	addConstant(&chunk, 420);
	assert(chunk.constants.capacity == 0x10);
	assert(chunk.constants.count == 0xe);
}

void testInterpretResult(void) {}

void test(void) {
	testConstant();
	testInterpretResult();
	shouldPrintHelloMax();
}

int main(void) { test(); }
