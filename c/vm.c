#include <stdio.h>

#include "debug.h"
#include "vm.h"

VM vm;

static void resetStack() { vm.stack.top = vm.stack.bottom; }

void initVM() {
  new_stack(&vm.stack);
  resetStack();
}

void freeVM() {}

InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op)                                                          \
  do {                                                                         \
    double b = stack_pop(&vm.stack);                                           \
    double a = stack_pop(&vm.stack);                                           \
    stack_push(&vm.stack, a op b);                                             \
  } while (false)

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("\t\t");
    if (stack_is_empty(&vm.stack))
      printf("[empty]");
    for (Value* slot = vm.stack.top - 1; slot >= vm.stack.bottom; --slot) {
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
      stack_push(&vm.stack, constant);
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
      Value constant = stack_pop(&vm.stack);
      stack_push(&vm.stack, -1 * constant);
      break;
    }
    case OP_RETURN:
      printValue(stack_pop(&vm.stack));
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
