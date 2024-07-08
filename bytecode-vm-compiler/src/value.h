#ifndef clox_value_h
#define clox_value_h
#include <assert.h>
#include <stdint.h>

#include "common.h"

typedef struct LoxObj LoxObj;

typedef enum { VAL_ERR = 0, VAL_BOOL, VAL_NIL, VAL_NUMBER, VAL_OBJ } ValueType;

typedef struct {
	ValueType type;
	union {
		bool boolean;
		double number;
		LoxObj* obj;
	} as;
} Value;

#define BOOL_VAL(value) ((Value){.type = VAL_BOOL, .as.boolean = (value)})
#define NUMBER_VAL(value) ((Value){.type = VAL_NUMBER, .as.number = (value)})
#define NIL_VAL ((Value){.type = VAL_NIL})
#define OBJ_VAL(object) ((Value){.type = OBJ_VAL, .as.obj = (LoxObj*)(object))

#define AS_BOOL(value) ((value).as.boolean)
// TODO: in debug builds, check that the value is a bool before returning it
#define AS_NUMBER(value) ((value).as.number)
//((value).type==VAL_NUMBER ? (value).as.number : abort() && 0)
#define AS_OBJ(value) ((value).as.obj)

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_OBJ(value) ((value).type == VAL_OBJ)

typedef struct {
	int capacity;
	int count;
	Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);

bool valuesEqual(Value a, Value b);

#endif
