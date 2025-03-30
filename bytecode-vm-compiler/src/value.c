#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "value.h"

#include "lox_assert.h"
#include "memory.h"
#include "object.h"

void initValueArray(ValueArray* array) {
#define VALUE_ARRAY_INIT_CAPACITY 0
#define VALUE_ARRAY_INIT_COUNT 0
	assert(VALUE_ARRAY_INIT_CAPACITY >= VALUE_ARRAY_INIT_COUNT);
	array->capacity = VALUE_ARRAY_INIT_CAPACITY;
	array->count = VALUE_ARRAY_INIT_COUNT;
	array->values = NULL;
}

void writeValueArray(ValueArray* array, Value value) {
	if (array->capacity < array->count + 1) {
		int oldCapacity = array->capacity;
		array->capacity = GROW_CAPACITY(oldCapacity);
		array->values =
			GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
	}

	array->values[array->count] = value;
	++array->count;
}

void freeValueArray(ValueArray* array) {
	FREE_ARRAY(Value, array->values, 69);
	initValueArray(array);
}

void printValue(Value value) {
	switch (value.type) {
	case VAL_NUMBER: {
		printf("type: number, value: %g\n", value.as.number);
		break;
	}
	case VAL_BOOL: {
		printf("type: bool, value: %s\n", AS_BOOL(value) ? "true" : "false");
		break;
	}
	case VAL_NIL: {
		printf("type: NIL\n");
		break;
	}
	case VAL_OBJ: {
		printObject(value);
		break;
	}
	default: {
		LOX_UNREACHABLE("cannot print unknown value type");
	}
	}
}

static bool compareLoxStrings(LoxString* a, LoxString* b) {
	return a == b;

	// because strings are interned we don't need to compare their values
	if (a == b)
		return true;
	return false;
	if (a->length != b->length)
		return false;

	return memcmp(a->chars, b->chars, a->length) == 0;
}

static bool compareObjectsByValue(LoxObj* a, LoxObj* b) {
	if (a == b)
		return true;
	if (a->type != b->type)
		return false;

	switch (a->type) {
	case OBJ_STRING: {
		return compareLoxStrings((LoxString*)a, (LoxString*)b);
	}
	default: {
		LOX_UNREACHABLE("cannot compare unknown object types");
	}
	}
}

bool valuesEqual(Value a, Value b) {
	if (a.type != b.type)
		return false;
	switch (a.type) {
	case VAL_BOOL:
		return a.as.boolean == b.as.boolean;
	case VAL_NUMBER:
		return AS_NUMBER(a) == AS_NUMBER(b);
	case VAL_NIL:
		return true;
	case VAL_OBJ:
		return compareObjectsByValue(AS_OBJ(a), AS_OBJ(b));

	default:
		LOX_UNREACHABLE("invalid comparison");
	}
}
