#include <stdio.h>
#include <stdlib.h>

#include "../lox_assert.h"
#include "../stack.h"

void basic() {
  Stack stack;
  new_stack(&stack);

  stack_push(&stack, 42);

  Value n = stack_pop(&stack);

  LOX_ASSERT(stack.cap == 1);
  LOX_ASSERT(n == 42);
}

void stack_grows() {
  Stack stack;
  new_stack(&stack);

  LOX_ASSERT(stack.cap == 1);
  stack_push(&stack, 1);
  stack_push(&stack, 2);
  LOX_ASSERT(stack.cap == 8);
  stack_push(&stack, 3);
  stack_push(&stack, 4);
  stack_push(&stack, 5);
  stack_push(&stack, 6);
  stack_push(&stack, 7);
  stack_push(&stack, 8);
  LOX_ASSERT(stack.cap == 8);
  stack_push(&stack, 9);
  LOX_ASSERT(stack.cap == 16);

  LOX_ASSERT(stack_pop(&stack) == 9.);
  LOX_ASSERT(stack_pop(&stack) == 8.);
  LOX_ASSERT(stack_pop(&stack) == 7.);
  LOX_ASSERT(stack_pop(&stack) == 6.);
  LOX_ASSERT(stack_pop(&stack) == 5.);
  LOX_ASSERT(stack_pop(&stack) == 4.);
  LOX_ASSERT(stack_pop(&stack) == 3.);
  LOX_ASSERT(stack_pop(&stack) == 2.);
  LOX_ASSERT(stack_pop(&stack) == 1.);
}

void push_and_pop() {
  Stack stack;
  new_stack(&stack);
  LOX_ASSERT(stack.cap == 1);
  stack_push(&stack, 1);
  stack_push(&stack, 2);
  LOX_ASSERT(stack.cap == 8);
  LOX_ASSERT(stack_pop(&stack) == 2);
  stack_push(&stack, 3);
  LOX_ASSERT(stack_pop(&stack) == 3);
  LOX_ASSERT(stack_pop(&stack) == 1);
}

int main() {
  basic();
  stack_grows();
  push_and_pop();
}
