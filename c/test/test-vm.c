#include "../src/chunk.h"
#include "../src/lox_assert.h"
#include "../src/vm.h"
#include <string.h>

/** -((1.2+3.4)/5.6)*/
void negativeOnepoint2PlusThreepoint4DividedbyFivepoint6(Chunk* chunk) {
	writeChunk(chunk, OP_CONSTANT, 1);
	int constant = addConstant(chunk, NUMBER_VAL(1.2));
	writeChunk(chunk, constant, 2);

	constant = addConstant(chunk, NUMBER_VAL(3.4));
	writeChunk(chunk, OP_CONSTANT, 1);
	writeChunk(chunk, constant, 42);

	writeChunk(chunk, OP_ADD, 1);

	constant = addConstant(chunk, NUMBER_VAL(5.6));
	writeChunk(chunk, OP_CONSTANT, 1);
	writeChunk(chunk, constant, 1);

	writeChunk(chunk, OP_DIVIDE, 1);

	writeChunk(chunk, OP_NEGATE, 2);

	writeChunk(chunk, OP_RETURN, 2);
}

/** 1*2+3 */
void OneTimesTwoPlusThree(Chunk* chunk) {
	int constant = addConstant(chunk, NUMBER_VAL(1));
	writeChunk(chunk, OP_CONSTANT, 2);
	writeChunk(chunk, constant, 2);

	constant = addConstant(chunk, NUMBER_VAL(2));
	writeChunk(chunk, OP_CONSTANT, 2);
	writeChunk(chunk, constant, 2);

	writeChunk(chunk, OP_MULTIPLY, 2);

	constant = addConstant(chunk, NUMBER_VAL(3));
	writeChunk(chunk, OP_CONSTANT, 2);
	writeChunk(chunk, constant, 2);

	writeChunk(chunk, OP_ADD, 2);

	writeChunkNoLine(chunk, OP_RETURN);
}

/** 1+2*3 */
void OnePlusTwoTimesThree(Chunk* chunk) {
	int constant = addConstant(chunk, NUMBER_VAL(1));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	constant = addConstant(chunk, NUMBER_VAL(2));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	constant = addConstant(chunk, NUMBER_VAL(3));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	writeChunkNoLine(chunk, OP_MULTIPLY);
	writeChunkNoLine(chunk, OP_ADD);
	writeChunkNoLine(chunk, OP_RETURN);
}

/** 3-2-1*/
void ThreeMinusTwoMinusOne(Chunk* chunk) {
	int constant = addConstant(chunk, NUMBER_VAL(3));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	constant = addConstant(chunk, NUMBER_VAL(2));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	writeChunkNoLine(chunk, OP_SUBTRACT);

	constant = addConstant(chunk, NUMBER_VAL(1));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	writeChunkNoLine(chunk, OP_SUBTRACT);

	writeChunkNoLine(chunk, OP_RETURN);
}

/** 1+2*3-4/-5*/
void OnePlusTwoTimesThreeMinusFourDivididedNegativeFive(Chunk* chunk) {
	int constant = addConstant(chunk, NUMBER_VAL(1));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	constant = addConstant(chunk, NUMBER_VAL(2));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	constant = addConstant(chunk, NUMBER_VAL(3));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	writeChunkNoLine(chunk, OP_MULTIPLY);
	writeChunkNoLine(chunk, OP_ADD);

	constant = addConstant(chunk, NUMBER_VAL(4));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	constant = addConstant(chunk, NUMBER_VAL(5));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	writeChunkNoLine(chunk, OP_NEGATE);
	writeChunkNoLine(chunk, OP_DIVIDE);
	writeChunkNoLine(chunk, OP_SUBTRACT);

	writeChunkNoLine(chunk, OP_RETURN);
}

void FourMinusThreeTimesNegativeTwo_without_negate(Chunk* chunk) {
	int constant = addConstant(chunk, NUMBER_VAL(4));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	constant = addConstant(chunk, NUMBER_VAL(3));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	constant = addConstant(chunk, NUMBER_VAL(0));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	constant = addConstant(chunk, NUMBER_VAL(2));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	writeChunkNoLine(chunk, OP_SUBTRACT);
	writeChunkNoLine(chunk, OP_MULTIPLY);
	writeChunkNoLine(chunk, OP_SUBTRACT);

	writeChunkNoLine(chunk, OP_RETURN);
}

void FourMinusThreeTimesNegativeTwo_without_subtract(Chunk* chunk) {
	int constant = addConstant(chunk, NUMBER_VAL(4));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	constant = addConstant(chunk, NUMBER_VAL(3));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	writeChunkNoLine(chunk, OP_NEGATE);

	constant = addConstant(chunk, NUMBER_VAL(2));
	writeChunkNoLine(chunk, OP_CONSTANT);
	writeChunkNoLine(chunk, constant);

	writeChunkNoLine(chunk, OP_NEGATE);
	writeChunkNoLine(chunk, OP_MULTIPLY);
	writeChunkNoLine(chunk, OP_ADD);

	writeChunkNoLine(chunk, OP_RETURN);
}
static Value peekStack(VM const* vm, int index) {
	// HACK: the top element of the stack should be `top-1`, but since
	// `OP_RETURN` pops the return value off the stack we're technically reading
	// "uninitalized" memory.  However, since we control the stack we know this
	// will have the value of the last thing set
	return *(vm->stack.top - index);
}

void addToStack(void) {
	initVM();
	Chunk chunk;
	initChunk(&chunk);
	{
		int constant = addConstant(&chunk, NUMBER_VAL(42));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);

		constant = addConstant(&chunk, NUMBER_VAL(69));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);

		writeChunkNoLine(&chunk, OP_RETURN);
	}
	interpret_bytecode_(&chunk);
	VM vm = getVM_();
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 0), NUMBER_VAL(69));
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 1), NUMBER_VAL(42));
}

void addition(void) {
	initVM();
	Chunk chunk;
	initChunk(&chunk);
	{
		int constant = addConstant(&chunk, NUMBER_VAL(42));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);

		constant = addConstant(&chunk, NUMBER_VAL(27));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);

		writeChunkNoLine(&chunk, OP_ADD);

		writeChunkNoLine(&chunk, OP_RETURN);
	}
	interpret_bytecode_(&chunk);
	VM vm = getVM_();
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 0), NUMBER_VAL(69));
}

void subtraction(void) {
	initVM();
	Chunk chunk;
	initChunk(&chunk);
	{
		int constant = addConstant(&chunk, NUMBER_VAL(7));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);

		constant = addConstant(&chunk, NUMBER_VAL(3));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);

		writeChunkNoLine(&chunk, OP_SUBTRACT);

		writeChunkNoLine(&chunk, OP_RETURN);
	}
	interpret_bytecode_(&chunk);
	VM vm = getVM_();
	LOX_ASSERT_EQUALS(peekStack(&vm, 0).type, VAL_NUMBER);
	LOX_ASSERT_EQUALS(peekStack(&vm, 0).as.number, 4);
}

static void assert_vm_exit_ok(InterpretResult result) {
	LOX_ASSERT_EQUALS(result, INTERPRET_OK);
}

void negation(void) {
	initVM();
	Chunk chunk;
	initChunk(&chunk);
	{
		int constant = addConstant(&chunk, NUMBER_VAL(7));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);

		writeChunkNoLine(&chunk, OP_NEGATE);

		writeChunkNoLine(&chunk, OP_RETURN);
	}
	InterpretResult status = interpret_bytecode_(&chunk);
	VM vm = getVM_();
	assert_vm_exit_ok(status);
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 0), NUMBER_VAL(-7));
}

void multiplication(void) {
	initVM();
	Chunk chunk;
	initChunk(&chunk);
	{
		int constant = addConstant(&chunk, NUMBER_VAL(210));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);

		constant = addConstant(&chunk, NUMBER_VAL(2));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);

		writeChunkNoLine(&chunk, OP_MULTIPLY);

		writeChunkNoLine(&chunk, OP_RETURN);
	}
	interpret_bytecode_(&chunk);
	VM vm = getVM_();
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 0), NUMBER_VAL(420));
}

void division(void) {
	initVM();
	Chunk chunk;
	initChunk(&chunk);
	{
		int constant = addConstant(&chunk, NUMBER_VAL(420));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);

		constant = addConstant(&chunk, NUMBER_VAL(10));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);

		writeChunkNoLine(&chunk, OP_DIVIDE);

		writeChunkNoLine(&chunk, OP_RETURN);
	}
	interpret_bytecode_(&chunk);
	VM vm = getVM_();
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 0), NUMBER_VAL(42));
}

int main(void) {
	addToStack();
	addition();
	subtraction();
	negation();
	multiplication();
	division();
}
