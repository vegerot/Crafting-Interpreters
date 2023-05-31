#include <stdlib.h>

#include "lox_assert.h"
#include "memory.h"
#include "stack.h"

void new_stack(Stack* stack) {
  stack->cap = STACK_INIT_SIZE;
  stack->top = stack->bottom = (Value*)malloc(sizeof(Value) * stack->cap);
}

void stack_push(Stack* stack, Value value) {
  LOX_ASSERT(stack->top - stack->bottom <= stack->cap && "stack overflow")
  if (stack->top - stack->bottom == stack->cap) {
    int oldSize = stack->cap;
    stack->cap = GROW_CAPACITY(oldSize);
    stack->bottom = GROW_ARRAY(Value, stack->bottom, oldSize, stack->cap);
    stack->top = stack->bottom + oldSize;
  }

  *stack->top = value;
  ++stack->top;
}

Value stack_pop(Stack* stack) {
  --stack->top;
  return *stack->top;
}

bool stack_is_empty(Stack* stack) { return stack->top == stack->bottom; }
