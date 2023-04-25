#include "value.h"
#include <assert.h>
#include <stdlib.h>

void initValueArray(ValueArray *array) {
#define VALUE_ARRAY_INIT_CAPACITY 0
#define VALUE_ARRAY_INIT_COUNT 0
  assert(VALUE_ARRAY_INIT_CAPACITY >= VALUE_ARRAY_INIT_COUNT);
  array->capacity = VALUE_ARRAY_INIT_CAPACITY;
  array->count = VALUE_ARRAY_INIT_COUNT;
  array->values = NULL;
}

void writeValueArray(ValueArray *array, Value value) {
  int count = array->count;
  int capacity = array->capacity;
  // just ==?
  if (count >= capacity) {
    // grow
	int newCap = capacity >= 8 ? 2 * capacity : 8;
    int newSize = sizeof(Value) * newCap;
	array->values = realloc(array->values, newSize);
    array->capacity = newCap;
  }

  array->values[array->count] = value;
  ++array->count;
}

void freeValueArray(ValueArray* array) {
  free(array->values);
  initValueArray(array);
}
