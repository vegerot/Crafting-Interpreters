#include "chunk.h"
#include "memory.h"

void initChunk(Chunk *chunk) {
  chunk->count = 0;
  /** should init `capacity` to 8 */
  (*chunk).capacity = 0;
  chunk->code = NULL;
}

void writeChunk(Chunk *chunk, uint8_t byte) {
  // could this just be ==?
  if (chunk->capacity <= chunk->count) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code =
        GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
  }
  chunk->code[chunk->count] = byte;
  ++chunk->count;
}

void freeChunk(Chunk *chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  initChunk(chunk);
}
