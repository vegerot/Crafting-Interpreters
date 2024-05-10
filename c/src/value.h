#ifndef clox_value_h
#define clox_value_h
#include "common.h"
#include <stdint.h>
#include <assert.h>

typedef enum { 
	VAL_BOOL,
	VAL_NIL,
	VAL_NUMBER
} ValueType;

typedef struct {
	union {
		bool boolean;
		double number;
	} as;
	ValueType type;
} Value;

typedef struct {
	struct {
		union {
			bool boolean;
			double number;
		} as;
		ValueType type;
		ValueType type2;
	};
} sixteenByteValue;

static_assert(sizeof(sixteenByteValue) == 16);

typedef struct {
	struct {
		union {
			bool boolean;
			double number;
		} as;
		ValueType type;
	};
	ValueType type2;
} twentyFourByteValue;

static_assert(sizeof(twentyFourByteValue) == 24);

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
