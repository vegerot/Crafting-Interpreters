#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "stack.h"

typedef struct {
	Chunk* chunk;
	/// actual location in memory of ip instead of offset into `chunk`
	uint8_t* ip;
	Stack stack;
} VM;

typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void initVM(void);
void freeVM(void);

VM getVM_(void);

InterpretResult interpret_bytecode_(Chunk* chunk);
InterpretResult interpret(char const* source);

#endif
