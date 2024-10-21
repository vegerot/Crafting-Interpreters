#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "compiler.h"
#include "debug.h"
#include "lox_assert.h"
#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

LoxString* allocateEmptyString(VM* vm, int length);
// TODO: make this a parameter
static VM vm; // NOLINT

static void resetStack(void) { vm.stack.top = vm.stack.bottom; }

static void runtimeError(char const* format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf( // NOLINT(clang-analyzer-valist.Uninitialized): bug in clang-tidy
		stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

	size_t instruction = vm.ip - vm.chunk->code - 1;
	int line = vm.chunk->lines[instruction];
	fprintf(stderr, "[line %d] in script\n", line);
	resetStack();
}

static bool isTruthy(Value value) {
	switch (value.type) {
	case VAL_BOOL:
		return value.as.boolean;
	case VAL_NIL:
		return false;
	case VAL_NUMBER:
		return AS_NUMBER(value) != 0.0;
	default:
		return true;
	}
}

void initVM(void) {
	new_stack(&vm.stack);
	resetStack();
	vm.objects = NULL;
}
void freeStack(Stack* stack) { free(stack->bottom); }

void freeVM(void) {
	freeObjects(&vm);
	freeStack(&vm.stack);
}

/**
 * hack: only used for tests
 */
VM getVM_(void) { return vm; }

InterpretResult run(void) {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(valueType, op)                                               \
	do {                                                                       \
		bool operands_have_same_type =                                         \
			stack_peek(&vm.stack, 0).type == stack_peek(&vm.stack, 1).type;    \
		if (!(operands_have_same_type)) {                                      \
			runtimeError("Operands must have same type to binary op");         \
			return INTERPRET_RUNTIME_ERROR;                                    \
		}                                                                      \
		Value b = stack_pop(&vm.stack);                                        \
		Value a = stack_pop(&vm.stack);                                        \
		switch (b.type) {                                                      \
		case VAL_NUMBER: {                                                     \
			double b_num = AS_NUMBER(b);                                       \
			double a_num = AS_NUMBER(a);                                       \
			stack_push(&vm.stack, valueType(a_num op b_num));                  \
			break;                                                             \
		}                                                                      \
		case VAL_OBJ: {                                                        \
			LoxObj* b_obj = AS_OBJ(b);                                         \
			LoxObj* a_obj = AS_OBJ(a);                                         \
			bool operands_have_same_obj_type = a_obj->type == b_obj->type;     \
			if (!(operands_have_same_obj_type)) {                              \
				/* maybe this should return false instead*/                    \
				runtimeError("Operands must have same obj type to binary op"); \
				return INTERPRET_RUNTIME_ERROR;                                \
			}                                                                  \
			switch (b_obj->type) {                                             \
			case OBJ_STRING: {                                                 \
				break;                                                         \
			}                                                                  \
			default: {                                                         \
				LOX_ASSERT(false &&                                            \
						   "unknown operand type to binary operation");        \
			}                                                                  \
			}                                                                  \
			break;                                                             \
		}                                                                      \
		default: {                                                             \
			LOX_ASSERT(false && "unknown operand type to binary operation");   \
		}                                                                      \
		}                                                                      \
	} while (false)

	for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
		printf("\nstack: \t\t");
		if (stack_is_empty(&vm.stack))
			printf("[empty]");
		for (Value* slot = vm.stack.top - 1; slot >= vm.stack.bottom; --slot) {
			printf("[ ");
			// TODO: wrap strings in quotes
			printValue(*slot);
			printf(" ]");
		}
		printf("\n");
		dissasembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
		if (vm.ip > vm.chunk->code + vm.chunk->capacity)
			LOX_ASSERT(false &&
					   "ran too much code.  Did you forget to OP_RETURN?");
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
		case OP_NIL:
			stack_push(&vm.stack, NIL_VAL);
			break;
		case OP_FALSE:
			stack_push(&vm.stack, BOOL_VAL(false));
			break;
		case OP_TRUE:
			stack_push(&vm.stack, BOOL_VAL(true));
			break;
		case OP_EQUAL: {
			Value b = stack_pop(&vm.stack);
			Value a = stack_pop(&vm.stack);
			stack_push(&vm.stack, BOOL_VAL(valuesEqual(a, b)));
			break;
		}
		case OP_GREATER:
			BINARY_OP(BOOL_VAL, >);
			break;
		case OP_LESS:
			BINARY_OP(BOOL_VAL, <);
			break;
		case OP_ADD: {
			bool operands_have_same_type =
				stack_peek(&vm.stack, 0).type == stack_peek(&vm.stack, 1).type;
			if (!(operands_have_same_type)) {
				runtimeError("Operands must have same type to binary op");
				return INTERPRET_RUNTIME_ERROR;
			}
			Value b = stack_pop(&vm.stack);
			Value a = stack_pop(&vm.stack);
			if (IS_NUMBER(b)) {
				double b_num = AS_NUMBER(b);
				double a_num = a.as.number;
				stack_push(&vm.stack, NUMBER_VAL(a_num + b_num));
			} else if (IS_STRING(b)) {
				LoxString* b_str = AS_STRING(b);
				LoxString* a_str = AS_STRING(a);
				size_t finalStringLength = b_str->length + a_str->length;
				LoxString* addedLoxStr =
					allocateEmptyString(&vm, (int)finalStringLength + 1);
				strncpy(addedLoxStr->chars, a_str->chars, a_str->length);
				strncpy(&addedLoxStr->chars[a_str->length], b_str->chars,
						b_str->length);
				// paranoid
				addedLoxStr->chars[finalStringLength] = '\0';
				stack_push(&vm.stack, OBJ_VAL(addedLoxStr));
			} else {
				runtimeError(
					"Operands to + must be two numbers or two strings");
			}
			break;
		}
		case OP_SUBTRACT:
			BINARY_OP(NUMBER_VAL, -);
			break;
		case OP_MULTIPLY:
			BINARY_OP(NUMBER_VAL, *);
			break;
		case OP_DIVIDE:
			BINARY_OP(NUMBER_VAL, /);
			break;
		case OP_NOT: {
			bool is_top_of_stack_truthy = isTruthy(stack_pop(&vm.stack));
			stack_push(&vm.stack, BOOL_VAL(!is_top_of_stack_truthy));
			break;
		}
		case OP_NEGATE: {
			// Value constant = stack_pop(&vm.stack);
			// stack_push(&vm.stack, NUMBER_VAL(-AS_NUMBER(constant)));
			// HACK: edit the stack directly
			// this is a bonus challenge in chapter 15
			Value top_of_stack = stack_peek(&vm.stack, 0);
			if (!IS_NUMBER(top_of_stack)) {
				runtimeError("Operand must be a number.");
				return INTERPRET_RUNTIME_ERROR;
			}
			(vm.stack.top - 1)->as.number *= -1;
			break;
		}
		case OP_RETURN:
			printValue(stack_pop(&vm.stack));
			printf("\n");
			return INTERPRET_OK;
		default:
			runtimeError("Unknown instruction: 0x%02x", instruction);
			return INTERPRET_RUNTIME_ERROR;
		}
	}
#undef READ_CONSTANT
#undef READ_BYTE
#undef BINARY_OP
}

InterpretResult interpret_bytecode_(Chunk* chunk) {
	vm.chunk = chunk;
	vm.ip = chunk->code;

	return run();
}

InterpretResult interpret(char const* source) {
	Chunk chunk;
	initChunk(&chunk);

	if (!compile(source, &chunk)) {
		freeChunk(&chunk);
		return INTERPRET_COMPILE_ERROR;
	}

	vm.chunk = &chunk;
	vm.ip = chunk.code;

	InterpretResult result = run();
	freeChunk(&chunk);
	freeVM();
	return result;
}
