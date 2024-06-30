#include <stdio.h>
#include <stdlib.h>

#include "../src/lox_assert.h"
#include "../src/stack.h"

void basic(void) {
	Stack stack;
	new_stack(&stack);

	stack_push(&stack, NUMBER_VAL(42));

	Value n = stack_pop(&stack);

	LOX_ASSERT(stack.cap == 1);
	LOX_ASSERT_EQUALS(n.type, VAL_NUMBER);
	LOX_ASSERT_EQUALS(n.as.number, 42);
	LOX_ASSERT_EQUALS(AS_NUMBER(n), 42);
	LOX_ASSERT_VALUE_EQUALS(n, NUMBER_VAL(42));
}

void stack_grows(void) {
	Stack stack;
	new_stack(&stack);

	LOX_ASSERT(stack.cap == 1);
	stack_push(&stack, NUMBER_VAL(1));
	stack_push(&stack, NUMBER_VAL(2));
	LOX_ASSERT(stack.cap == 8);
	stack_push(&stack, NUMBER_VAL(3));
	stack_push(&stack, NUMBER_VAL(4));
	stack_push(&stack, NUMBER_VAL(5));
	stack_push(&stack, NUMBER_VAL(6));
	stack_push(&stack, NUMBER_VAL(7));
	stack_push(&stack, NUMBER_VAL(8));
	LOX_ASSERT(stack.cap == 8);
	stack_push(&stack, NUMBER_VAL(9));
	LOX_ASSERT(stack.cap == 16);

	LOX_ASSERT_VALUE_EQUALS(stack_pop(&stack), NUMBER_VAL(9.));
	LOX_ASSERT_VALUE_EQUALS(stack_pop(&stack), NUMBER_VAL(8.));
	LOX_ASSERT_VALUE_EQUALS(stack_pop(&stack), NUMBER_VAL(7.));
	LOX_ASSERT_VALUE_EQUALS(stack_pop(&stack), NUMBER_VAL(6.));
	LOX_ASSERT_VALUE_EQUALS(stack_pop(&stack), NUMBER_VAL(5.));
	LOX_ASSERT_VALUE_EQUALS(stack_pop(&stack), NUMBER_VAL(4.));
	LOX_ASSERT_VALUE_EQUALS(stack_pop(&stack), NUMBER_VAL(3.));
	LOX_ASSERT_VALUE_EQUALS(stack_pop(&stack), NUMBER_VAL(2.));
	LOX_ASSERT_VALUE_EQUALS(stack_pop(&stack), NUMBER_VAL(1.));
}

void push_and_pop(void) {
	Stack stack;
	new_stack(&stack);
	LOX_ASSERT(stack.cap == 1);
	stack_push(&stack, NUMBER_VAL(1));
	stack_push(&stack, NUMBER_VAL(2));
	LOX_ASSERT(stack.cap == 8);
	LOX_ASSERT_VALUE_EQUALS(stack_pop(&stack), NUMBER_VAL(2));
	stack_push(&stack, NUMBER_VAL(3));
	LOX_ASSERT_VALUE_EQUALS(stack_pop(&stack), NUMBER_VAL(3));
	LOX_ASSERT_VALUE_EQUALS(stack_pop(&stack), NUMBER_VAL(1));
}

int main(void) {
	basic();
	stack_grows();
	push_and_pop();
}
