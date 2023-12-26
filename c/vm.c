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
    Value b = stack_pop(&vm.stack);                                           \
    Value a = stack_pop(&vm.stack);                                           \
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
#ifdef DEBUG_TRACE_EXECUTION
      printf("constant at 0x%ld = ", (vm.ip - 1) - vm.chunk->code);
      printValue(constant);
      printf("\n");
#endif
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
        // Value constant = stack_pop(&vm.stack);
        // stack_push(&vm.stack, -1 * constant);
        // HACK: edit the stack directly
        // this is a bonus challenge in chapter 15
        *(vm.stack.top-1) *= -1;
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
