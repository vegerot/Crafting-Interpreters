#include <stdio.h>

#include "vm.h"

VM vm;

void initVM() {}

void freeVM() {}

InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
  for (;;) {
    uint8_t instruction = READ_BYTE();
    switch (instruction) {
    case OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      printf("constant at 0x%ld = ", (vm.ip - 1) - vm.chunk->code);
      printValue(constant);
      printf("\n");
    }
    case OP_RETURN:
      return INTERPRET_OK;
    default:
      return INTERPRET_RUNTIME_ERROR;
    }
  }
#undef READ_CONSTANT
#undef READ_BYTE
}

InterpretResult interpret(Chunk* chunk) {
  vm.chunk = chunk;
  vm.ip = chunk->code;

  return run();
}
