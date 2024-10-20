#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lox_assert.h"
#include "memory.h"
#include "object.h"

void printObject(Value value) {
	switch (OBJ_TYPE(value)) {
	case OBJ_STRING:
		printf("%.*s", AS_STRING(value)->length, AS_CSTRING(value));
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

LoxString* allocateString(VM* vm, char const* cString, int length) {
	LoxString* str = (LoxString*)allocateObj(
		vm, sizeof(LoxString) + (length + 1) * sizeof(char), OBJ_STRING);

	strncpy(str->chars, cString, length);
	str->chars[length + 1] = '\0';

	str->length = length;
	return str;
}
