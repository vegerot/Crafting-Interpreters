#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
	OP_CONSTANT,
	OP_NIL,
	OP_TRUE,
	OP_FALSE,
	OP_EQUAL,
	OP_GREATER,
	OP_LESS,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_CONDITIONAL_PLACEHOLDER, // See Note[ConditionalOperator]
	OP_DIVIDE,
	OP_NOT,
	OP_NEGATE,
	OP_RETURN,
} OpCode;

typedef struct {
	int count;
	int capacity;
	uint8_t* code;
	int* lines;
	LoxObj* allocatorStart;
	ValueArray constants;
} Chunk;

void initChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
/** `writeChunk` and set line number to -1*/
void writeChunkNoLine(Chunk* chunk, uint8_t byte);
void freeChunk(Chunk* chunk);
int addConstant(Chunk* chunk, Value value);

#endif
