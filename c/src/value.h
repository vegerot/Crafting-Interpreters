#ifndef clox_value_h
#define clox_value_h
#include "common.h"
#include <assert.h>
#include <stdint.h>

typedef enum { VAL_BOOL, VAL_NIL, VAL_NUMBER } ValueType;

typedef struct {
	ValueType type;
	union {
		bool boolean;
		double number;
	} as;
} Value;

#define BOOL_VAL(value) ((Value){.type=VAL_VAL_BOOL, .as.bool=(value)})
#define NUMBER_VAL(value) ((Value){.type=VAL_NUMBER, .as.number=(value)})
#define NIL_VAL ((Value){.type=VAL_NIL})

#define AS_BOOL(value) ((value).as.bool)
#define AS_NUMBER(value) ((value).as.number)

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_NIL(value) ((value).type == VAL_NIL)

typedef struct {
	int capacity;
	int count;
	Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);

#endif
