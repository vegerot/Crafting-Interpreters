#include "chunk.h"
#include "common.h"
#include "debug.h"

#include "value.h"
#include <stdio.h>

int main(int argc, const char *argv[]) {
  /*
  Chunk chunk;
  initChunk(&chunk);
  writeChunk(&chunk, OP_RETURN);
  // want this to fail
  writeChunk(&chunk, 7);
  dissasembleChunk(&chunk, "test chunk");
  freeChunk(&chunk);
  */

  ValueArray array;
  initValueArray(&array);
  writeValueArray(&array, 69.0);
  writeValueArray(&array, 420);
  writeValueArray(&array, 69.0);
  writeValueArray(&array, 420);
  writeValueArray(&array, 69.0);
  writeValueArray(&array, 420);
  writeValueArray(&array, 69.0);
  writeValueArray(&array, 420);
  writeValueArray(&array, 69.0);
  writeValueArray(&array, 420);
  writeValueArray(&array, 69.0);
  writeValueArray(&array, 420);
  writeValueArray(&array, 69.0);
  writeValueArray(&array, 420);
  printf("array.capacity: %x, array.count: %x\n", array.capacity, array.count);
  for (int i = 0; i<array.capacity; ++i) {
	printf("array.values[%d] = %f\n", i, array.values[i]);
  }
  freeValueArray(&array);
  return 0;
}
