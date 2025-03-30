#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
	chunk->count = 0;
	/** FIXME: should init `capacity` to 8 */
	(*chunk).capacity = 0;
	chunk->code = NULL;
	chunk->lines = NULL;
	chunk->allocatorStart = NULL;
	initValueArray(&(chunk->constants));
}

void writeChunkNoLine(Chunk* chunk, uint8_t byte) {
	writeChunk(chunk, byte, 0);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
	// could this just be ==?
	if (chunk->capacity <= chunk->count) {
		int oldCapacity = chunk->capacity;
		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code =
			GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
		chunk->lines =
			GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
	}
	chunk->code[chunk->count] = byte;
	chunk->lines[chunk->count] = line;
	++chunk->count;
}

int addConstant(Chunk* chunk, Value value) {
	writeValueArray(&chunk->constants, value);
	return chunk->constants.count - 1;
}

static void freeAllocator(LoxObj* start) {
	LoxObj* curr = start;
#ifdef DEBUG_TRACE_EXECUTION
	static int i = 0;
#endif
	while (curr) {
#ifdef DEBUG_TRACE_EXECUTION
		printf("Freeing %d: %p\n", i++, (void*)curr);
#endif
		LoxObj* next = curr->next;
		free(curr);
		curr = next;
	}
	return;
}

void freeChunk(Chunk* chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	FREE_ARRAY(int, chunk->lines, chunk->capacity);
	freeValueArray(&((*chunk).constants));
	freeAllocator(chunk->allocatorStart);
	initChunk(chunk);
}
