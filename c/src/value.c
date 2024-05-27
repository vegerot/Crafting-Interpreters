#include "value.h"
#include "memory.h"
#include "lox_assert.h"
#include <stdio.h>
#include <stdlib.h>

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
		printf("type: bool, value: %s\n", value.as.boolean ? "true" : "false");
		break;
	}
	case VAL_NIL: {
		printf("type: NIL\n");
		break;
	}
	case VAL_ERR: {
			LOX_ASSERT(false && "wtf?");
		}
	}
}
