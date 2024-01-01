#ifndef lox_stack_h
#define lox_stack_h
#define STACK_MAX 256
#define STACK_INIT_SIZE 1
#include "value.h"
#include <stdbool.h>

typedef struct {
	int cap;
	/**
	 * actual location in memory of stack top instead of offset into `stack`
	 */
	Value* top;
	Value* bottom;
} Stack;

void new_stack(Stack* stack);
void stack_push(Stack* stack, Value value);
Value stack_pop(Stack* stack);
bool stack_is_empty(Stack* stack);
#endif
