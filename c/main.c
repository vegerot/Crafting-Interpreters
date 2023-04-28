#include <stdio.h>

#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "value.h"
#include "vm.h"

int main() {
  initVM();
  Chunk chunk;
  initChunk(&chunk);
  writeChunk(&chunk, OP_CONSTANT, 1);
  int constant = addConstant(&chunk, 1.2);
  writeChunk(&chunk, constant, 2);

  writeChunk(&chunk, OP_RETURN, 2);
  // want this to fail
  // writeChunk(&chunk, 7, 0);
  dissasembleChunk(&chunk, "test chunk");

  interpret(&chunk);

  freeVM();
  freeChunk(&chunk);
  return 0;
}
