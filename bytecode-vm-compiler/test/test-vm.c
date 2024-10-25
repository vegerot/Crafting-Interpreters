#include "../src/chunk.h"
#include "../src/compiler.h"
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
	freeChunk(&chunk);
	freeVM();
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
	freeChunk(&chunk);
	freeVM();
}

void additionWithCompile(void) {
	initVM();
	Chunk chunk;
	initChunk(&chunk);

	compile("1+2", &chunk);

	InterpretResult result = interpret_bytecode_(&chunk);

	LOX_ASSERT_EQUALS(result, INTERPRET_OK);

	VM vm = getVM_();

	int want = 3;

	Value go = peekStack(&vm, 0);

	LOX_ASSERT_VALUE_EQUALS(go, NUMBER_VAL(want));
	freeChunk(&chunk);
	freeVM();
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
	freeChunk(&chunk);
	freeVM();
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
	freeChunk(&chunk);
	freeVM();
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
	freeChunk(&chunk);
	freeVM();
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
	freeChunk(&chunk);
	freeVM();
}

void not(void) {
	initVM();
	Chunk chunk;
	initChunk(&chunk);
	{
		writeChunkNoLine(&chunk, OP_TRUE);
		writeChunkNoLine(&chunk, OP_NOT);

		writeChunkNoLine(&chunk, OP_TRUE);
		writeChunkNoLine(&chunk, OP_NOT);
		writeChunkNoLine(&chunk, OP_NOT);

		writeChunkNoLine(&chunk, OP_FALSE);
		writeChunkNoLine(&chunk, OP_NOT);

		writeChunkNoLine(&chunk, OP_NIL);
		writeChunkNoLine(&chunk, OP_NOT);

		int constant = addConstant(&chunk, NUMBER_VAL(0));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		writeChunkNoLine(&chunk, OP_NOT);
		writeChunkNoLine(&chunk, OP_NOT);

		constant = addConstant(&chunk, NUMBER_VAL(1));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		writeChunkNoLine(&chunk, OP_NOT);
		writeChunkNoLine(&chunk, OP_NOT);

		constant = addConstant(&chunk, NUMBER_VAL(-1));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		writeChunkNoLine(&chunk, OP_NOT);
		writeChunkNoLine(&chunk, OP_NOT);

		writeChunkNoLine(&chunk, OP_RETURN);
	}
	interpret_bytecode_(&chunk);
	VM vm = getVM_();
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 6), BOOL_VAL(false));
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 5), BOOL_VAL(true));
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 4), BOOL_VAL(true));
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 3), BOOL_VAL(true));
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 2), BOOL_VAL(false));
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 1), BOOL_VAL(true));
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 0), BOOL_VAL(true));
	freeChunk(&chunk);
	freeVM();
}

static void comparison() {
	initVM();
	Chunk chunk;
	initChunk(&chunk);
	{
		writeChunkNoLine(&chunk, OP_TRUE);
		writeChunkNoLine(&chunk, OP_TRUE);
		writeChunkNoLine(&chunk, OP_EQUAL);

		int constant = addConstant(&chunk, NUMBER_VAL(0));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		constant = addConstant(&chunk, NUMBER_VAL(1));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		writeChunkNoLine(&chunk, OP_EQUAL);

		constant = addConstant(&chunk, NUMBER_VAL(0));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		writeChunkNoLine(&chunk, OP_FALSE);
		writeChunkNoLine(&chunk, OP_EQUAL);

		constant = addConstant(&chunk, NUMBER_VAL(0.1));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		constant = addConstant(&chunk, NUMBER_VAL(1.5));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		writeChunkNoLine(&chunk, OP_GREATER);

		constant = addConstant(&chunk, NUMBER_VAL(69.42));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		constant = addConstant(&chunk, NUMBER_VAL(42.69));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		writeChunkNoLine(&chunk, OP_GREATER);

		constant = addConstant(&chunk, NUMBER_VAL(1.5));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		constant = addConstant(&chunk, NUMBER_VAL(0.1));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		writeChunkNoLine(&chunk, OP_LESS);

		constant = addConstant(&chunk, NUMBER_VAL(42.69));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		constant = addConstant(&chunk, NUMBER_VAL(69.42));
		writeChunkNoLine(&chunk, OP_CONSTANT);
		writeChunkNoLine(&chunk, constant);
		writeChunkNoLine(&chunk, OP_LESS);

		writeChunkNoLine(&chunk, OP_RETURN);
	}
	InterpretResult result = interpret_bytecode_(&chunk);
	LOX_ASSERT_EQUALS(result, INTERPRET_OK);
	VM vm = getVM_();

	// true==true is true
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 6), BOOL_VAL(true));
	// 0==1 is false
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 5), BOOL_VAL(false));
	// 0==false is false
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 4), BOOL_VAL(false));
	// 0.1>1.5 is false
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 3), BOOL_VAL(false));
	// 69.42 > 42.69 is true
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 2), BOOL_VAL(true));
	// 1.5 < 0.1 is false
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 1), BOOL_VAL(false));
	// 42.69 < 69.42 is true
	LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 0), BOOL_VAL(true));
	freeChunk(&chunk);
	freeVM();
}

static void stringComp() {
	Chunk chunk;
	{
		initVM();
		initChunk(&chunk);

		compile("\"abc\" == \"abc\"", &chunk);

		InterpretResult result = interpret_bytecode_(&chunk);

		LOX_ASSERT_EQUALS(result, INTERPRET_OK);

		VM vm = getVM_();

		LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 0), BOOL_VAL(true));
		freeChunk(&chunk);
		freeVM();
	}
	{
		initVM();
		initChunk(&chunk);

		compile("\"abc\" == \"def\"", &chunk);
		freeChunk(&chunk);
		return;

		InterpretResult result = interpret_bytecode_(&chunk);

		LOX_ASSERT_EQUALS(result, INTERPRET_OK);

		VM vm = getVM_();

		LOX_ASSERT_VALUE_EQUALS(peekStack(&vm, 0), BOOL_VAL(false));
	}
}

static void stringAdd() {
	Chunk chunk;
	initChunk(&chunk);

	compile("\"string\" + \" append\"", &chunk);

	InterpretResult result = interpret_bytecode_(&chunk);

	LOX_ASSERT_EQUALS(result, INTERPRET_OK);

	VM vm = getVM_();

	char const* want = "string append";

	char* got = AS_CSTRING(peekStack(&vm, 0));

	LOX_ASSERT_STRING_EQUALS(got, want);
	freeChunk(&chunk);
	freeVM();
}

int main(void) {
	addToStack();
	addition();
	additionWithCompile();
	subtraction();
	negation();
	multiplication();
	division();
	not();
	comparison();
	stringComp();
	stringAdd();
}
