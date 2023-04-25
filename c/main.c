#include "chunk.h"
#include "common.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
  Chunk chunk;
  initChunk(&chunk);
  writeChunk(&chunk, OP_RETURN);
  // want this to fail
  writeChunk(&chunk, 7);
  dissasembleChunk(&chunk, "test chunk");
  freeChunk(&chunk);


  return 0;
}
