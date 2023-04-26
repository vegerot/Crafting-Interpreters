#include "chunk.h"
#include "common.h"
#include "debug.h"

#include "value.h"
#include <stdio.h>

int main() {
  Chunk chunk;
  initChunk(&chunk);
  writeChunk(&chunk, OP_CONSTANT, 1);
  int constant = addConstant(&chunk, 1.2);
  writeChunk(&chunk, constant, 2);

  writeChunk(&chunk, OP_RETURN, 2);
  // want this to fail
  // writeChunk(&chunk, 7, 0);
  dissasembleChunk(&chunk, "test chunk");
  printf("\n");

  addConstant(&chunk, 69.0);
  addConstant(&chunk, 420);
  addConstant(&chunk, 69.0);
  addConstant(&chunk, 420);
  addConstant(&chunk, 69.0);
  addConstant(&chunk, 420);
  addConstant(&chunk, 69.0);
  addConstant(&chunk, 420);
  addConstant(&chunk, 69.0);
  addConstant(&chunk, 420);
  addConstant(&chunk, 69.0);
  addConstant(&chunk, 420);
  addConstant(&chunk, 69.0);
  addConstant(&chunk, 420);
  printf("chunk.constants.capacity: 0x%x, array.count: 0x%x\n",
         chunk.constants.capacity, chunk.constants.count);
  for (int i = 0; i < chunk.constants.capacity; ++i) {
    printf("chunk.constants.values[%d] = %g\n", i, chunk.constants.values[i]);
  }
  freeChunk(&chunk);
  return 0;
}
