#include <stdio.h>

#include "debug.h"
#include "vm.h"

VM vm;

static void resetStack() { vm.stackTop = vm.stack; }

void initVM() { resetStack(); }

void freeVM() {}

InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op)                                                          \
  do {                                                                         \
    double b = pop();                                                          \
    double a = pop();                                                          \
    push(a op b);                                                              \
  } while (false)

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("\t\t");
    if (isStackEmpty())
      printf("[empty]");
    for (Value* slot = vm.stackTop - 1; slot >= vm.stack; --slot) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");
    dissasembleInstruction(vm.chunk, vm.ip - vm.chunk->code);
#endif
    uint8_t instruction = READ_BYTE();
    switch (instruction) {
    case OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      push(constant);
      printf("constant at 0x%ld = ", (vm.ip - 1) - vm.chunk->code);
      printValue(constant);
      printf("\n");
      break;
    }
    case OP_ADD:
      BINARY_OP(+);
      break;
    case OP_SUBTRACT:
      BINARY_OP(-);
      break;
    case OP_MULTIPLY:
      BINARY_OP(*);
      break;
    case OP_DIVIDE:
      BINARY_OP(/);
      break;
    case OP_NEGATE: {
      Value constant = pop();
      push(-1 * constant);
      break;
    }
    case OP_RETURN:
      printValue(pop());
      printf("\n");
      return INTERPRET_OK;
    default:
      return INTERPRET_RUNTIME_ERROR;
    }
  }
#undef READ_CONSTANT
#undef READ_BYTE
#undef BINARY_OP
}

InterpretResult interpret(Chunk* chunk) {
  vm.chunk = chunk;
  vm.ip = chunk->code;

  return run();
}

void push(Value value) {
  *vm.stackTop = value;
  ++vm.stackTop;
}

Value pop() {
  --vm.stackTop;
  return *vm.stackTop;
}

bool isStackEmpty() { return vm.stackTop == vm.stack; }
