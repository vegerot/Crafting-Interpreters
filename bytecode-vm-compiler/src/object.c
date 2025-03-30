#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lox_assert.h"
#include "memory.h"
#include "object.h"

void printObject(Value value) {
	switch (OBJ_TYPE(value)) {
	case OBJ_STRING:
		printf("%.*s", (int)AS_STRING(value)->length, AS_CSTRING(value));
		break;
	default:
		printf("\n obj type: %u\n", OBJ_TYPE(value));
		LOX_UNREACHABLE("cannot print this object type");
	}
}

#define ALLOCATE_OBJ(type, objectType)                                         \
	(type*)allocateObj(sizeof(type), objectType)

static LoxObj* allocateObj(VM* vm, size_t size, ObjType type) {
	LoxObj* object = (LoxObj*)reallocate(NULL, size);
	object->type = type;
	object->next = vm->objects;
	vm->objects = object;
	return object;
}

void internString(VM* vm, LoxString* str) {
	tableSet(&vm->strings, str, NIL_VAL);
}

/*
 * NOTE: you must remember to intern the string after filling it!
 */
LoxString* newEmptyLoxString(VM* vm, size_t length) {
	LoxString* str = (LoxString*)allocateObj(
		vm, sizeof(LoxString) + (length + 1) * sizeof(char), OBJ_STRING);

	// This line does NOTHING.
	// See NOTE[why-LoxString-null-terminates]
	str->chars[length] = '\0';

	str->length = length;
	return str;
}

LoxString* newLoxStringFromCStringAndHash(VM* vm, char const* cString,
										  size_t length, uint32_t hash) {
	LoxString* maybeInterned =
		tableFindString(&vm->strings, cString, length, hash);
	if (maybeInterned != NULL)
		return maybeInterned;
	LoxString* str = newEmptyLoxString(vm, length);

	strncpy(str->chars, cString, length);
	str->hash = hash;
	internString(vm, str);
	return str;
}

LoxString* newLoxStringFromCString(VM* vm, char const* cString, size_t length) {
	uint32_t hash = computeHashOfCString(cString, length);
	return newLoxStringFromCStringAndHash(vm, cString, length, hash);
}

uint32_t computeHashOfCString(char const* chars, size_t length) {
	uint32_t hash = 2166136261u;
	for (size_t i = 0; i < length; ++i) {
		hash ^= (uint8_t)chars[i];
		hash *= 16777619;
	}
	return hash;
}
