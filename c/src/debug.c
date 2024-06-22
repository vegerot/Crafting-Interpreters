#include "debug.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>

void dissasembleChunk(Chunk* chunk, char const* name) {
	printf("== %s ==\n", name);

	for (int offset = 0; offset < chunk->count;) {
		offset = dissasembleInstruction(chunk, offset);
	}
}

static int constantInstruction(char const* name, Chunk* chunk, int offset) {
	uint8_t constantIndex = chunk->code[offset + 1];
	printf("%-16s %4d ", name, constantIndex);
	printValue(chunk->constants.values[constantIndex]);
	printf("\n");
	return offset + 2;
}

static int simpleInstruction(char const* name, int offset) {
	printf("%s\n", name);
	return offset + 1;
}

int dissasembleInstruction(Chunk* chunk, int offset) {
	printf("0x%04x ", offset);
	if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
		printf("   | ");
	} else {
		printf("%4x ", chunk->lines[offset]);
	}

	uint8_t instruction = chunk->code[offset];

	switch (instruction) {
	case OP_CONSTANT:
		return constantInstruction("OP_CONSTANT", chunk, offset);
	case OP_NIL:
		return simpleInstruction("OP_NIL", offset);
	case OP_TRUE:
		return simpleInstruction("OP_TRUE", offset);
	case OP_FALSE:
		return simpleInstruction("OP_FALSE", offset);
	case OP_ADD:
		return simpleInstruction("OP_ADD", offset);
	case OP_SUBTRACT:
		return simpleInstruction("OP_SUBTRACT", offset);
	case OP_MULTIPLY:
		return simpleInstruction("OP_MULTIPLY", offset);
	case OP_DIVIDE:
		return simpleInstruction("OP_DIVIDE", offset);
	case OP_NOT:
		return simpleInstruction("OP_NOT", offset);
	case OP_NEGATE:
		return simpleInstruction("OP_NEGATE", offset);
	case OP_RETURN:
		return simpleInstruction("OP_RETURN", offset);
	default:
		fflush(stdout);
		fprintf(stderr, "Unknown opcode 0x%x\n", instruction);
		exit(1);
	}
}
