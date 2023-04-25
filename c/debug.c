#include "debug.h"
#include <stdio.h>
#include <stdlib.h>

void dissasembleChunk(Chunk *chunk, const char *name) {
  printf("== %s ==\n", name);

  for (int offset = 0; offset < chunk->count;) {
    offset = dissasembleInstruction(chunk, offset);
  }
}

static int simpleInstruction(const char *name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}

int dissasembleInstruction(Chunk *chunk, int offset) {
  printf("%04x ", offset);

  uint8_t instruction = chunk->code[offset];

  switch (instruction) {
  case OP_RETURN:
    return simpleInstruction("OP_RETURN", offset);
  default:
    fflush(stdout);
    fprintf(stderr, "Unknown opcode 0x%x\n", instruction);
    exit(1);
  }
}
