#include <stdio.h>

#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "value.h"
#include "vm.h"

/** -((1.2+3.4)/5.6)*/
void negativeOnepoint2PlusThreepoint4DividedbyFivepoint6(Chunk* chunk) {
  writeChunk(chunk, OP_CONSTANT, 1);
  int constant = addConstant(chunk, 1.2);
  writeChunk(chunk, constant, 2);

  constant = addConstant(chunk, 3.4);
  writeChunk(chunk, OP_CONSTANT, 1);
  writeChunk(chunk, constant, 42);

  writeChunk(chunk, OP_ADD, 1);

  constant = addConstant(chunk, 5.6);
  writeChunk(chunk, OP_CONSTANT, 1);
  writeChunk(chunk, constant, 1);

  writeChunk(chunk, OP_DIVIDE, 1);

  writeChunk(chunk, OP_NEGATE, 2);

  writeChunk(chunk, OP_RETURN, 2);
}

int main() {
  initVM();
  Chunk chunk;
  initChunk(&chunk);
  negativeOnepoint2PlusThreepoint4DividedbyFivepoint6(&chunk);
  printf("\nINTERPRET BYTECODE:\n");
  interpret(&chunk);

  freeVM();
  freeChunk(&chunk);
  return 0;
}
