#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "object.h"
#include "stack.h"
#include "table.h"

typedef struct {
	Chunk* chunk;
	/// actual location in memory of ip instead of offset into `chunk`
	uint8_t* ip;
	Stack stack;
	LoxTable strings;
	LoxObj* objects;
} VM;

typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void initVM(void);
void freeVM(void);

VM* getVM_(void);

InterpretResult interpret_bytecode_(Chunk* chunk);
InterpretResult interpret(char const* source);

LoxString* newLoxStringFromCString(VM* vm, char const* cString, size_t length);
LoxString* newLoxStringFromCStringAndHash(VM* vm, char const* cString,
										  size_t length, uint32_t hash);

#endif
