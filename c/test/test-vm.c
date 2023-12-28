#include "../chunk.h"
#include "../lox_assert.h"
#include "../vm.h"

Value peekStack(VM* vm, int index) {
  // HACK: the top element of the stack should be `top-1`, but since `OP_RETURN`
  // pops the return value off the stack we're technically reading
  // "uninitalized" memory.  However, since we control the stack we know this
  // will have the value of the last thing set
  return *(vm->stack.top - index);
}

void addToStack(void) {
  initVM();
  Chunk chunk;
  initChunk(&chunk);
  {
    int constant = addConstant(&chunk, 42);
    writeChunkNoLine(&chunk, OP_CONSTANT);
    writeChunkNoLine(&chunk, constant);

    constant = addConstant(&chunk, 69);
    writeChunkNoLine(&chunk, OP_CONSTANT);
    writeChunkNoLine(&chunk, constant);

    writeChunkNoLine(&chunk, OP_RETURN);
  }
  interpret(&chunk);
  VM vm = _getVM();
  LOX_ASSERT(peekStack(&vm, 0) == 69);
  LOX_ASSERT(peekStack(&vm, 1) == 42);
}

void addition(void) {
  initVM();
  Chunk chunk;
  initChunk(&chunk);
  {
    int constant = addConstant(&chunk, 42);
    writeChunkNoLine(&chunk, OP_CONSTANT);
    writeChunkNoLine(&chunk, constant);

    constant = addConstant(&chunk, 27);
    writeChunkNoLine(&chunk, OP_CONSTANT);
    writeChunkNoLine(&chunk, constant);

    writeChunkNoLine(&chunk, OP_ADD);

    writeChunkNoLine(&chunk, OP_RETURN);
  }
  interpret(&chunk);
  VM vm = _getVM();
  LOX_ASSERT(peekStack(&vm, 0) == 69);
}

void subtraction(void) {
  initVM();
  Chunk chunk;
  initChunk(&chunk);
  {
    int constant = addConstant(&chunk, 7);
    writeChunkNoLine(&chunk, OP_CONSTANT);
    writeChunkNoLine(&chunk, constant);

    constant = addConstant(&chunk, 3);
    writeChunkNoLine(&chunk, OP_CONSTANT);
    writeChunkNoLine(&chunk, constant);

    writeChunkNoLine(&chunk, OP_SUBTRACT);

    writeChunkNoLine(&chunk, OP_RETURN);
  }
  interpret(&chunk);
  VM vm = _getVM();
  LOX_ASSERT(peekStack(&vm, 0) == 4);
}

void negation(void) {
  initVM();
  Chunk chunk;
  initChunk(&chunk);
  {
    int constant = addConstant(&chunk, 7);
    writeChunkNoLine(&chunk, OP_CONSTANT);
    writeChunkNoLine(&chunk, constant);

    writeChunkNoLine(&chunk, OP_NEGATE);

    writeChunkNoLine(&chunk, OP_RETURN);
  }
  interpret(&chunk);
  VM vm = _getVM();
  LOX_ASSERT(peekStack(&vm, 0) == -7);
}

void multiplication(void) {
  initVM();
  Chunk chunk;
  initChunk(&chunk);
  {
    int constant = addConstant(&chunk, 210);
    writeChunkNoLine(&chunk, OP_CONSTANT);
    writeChunkNoLine(&chunk, constant);

    constant = addConstant(&chunk, 2);
    writeChunkNoLine(&chunk, OP_CONSTANT);
    writeChunkNoLine(&chunk, constant);

    writeChunkNoLine(&chunk, OP_MULTIPLY);

    writeChunkNoLine(&chunk, OP_RETURN);
  }
  interpret(&chunk);
  VM vm = _getVM();
  LOX_ASSERT(peekStack(&vm, 0) == 420);
}

void division(void) {
  initVM();
  Chunk chunk;
  initChunk(&chunk);
  {
    int constant = addConstant(&chunk, 420);
    writeChunkNoLine(&chunk, OP_CONSTANT);
    writeChunkNoLine(&chunk, constant);

    constant = addConstant(&chunk, 10);
    writeChunkNoLine(&chunk, OP_CONSTANT);
    writeChunkNoLine(&chunk, constant);

    writeChunkNoLine(&chunk, OP_DIVIDE);

    writeChunkNoLine(&chunk, OP_RETURN);
  }
  interpret(&chunk);
  VM vm = _getVM();
  LOX_ASSERT(peekStack(&vm, 0) == 42);
}

int main(void) {
  addToStack();
  addition();
  subtraction();
  negation();
  multiplication();
  division();
}
