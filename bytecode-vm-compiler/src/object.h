#pragma once

#include "common.h"
#include "value.h"

#define OBJ_TYPE(value) (AS_OBJ(value)->type)

#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_STRING(value) ((LoxString*)(value).as.obj)
#define AS_CSTRING(value) (AS_STRING(value)->chars)

typedef enum {
	OBJ_ERROR,
	OBJ_STRING,
} ObjType;

struct LoxObj {
	ObjType type;
	struct LoxObj* next;
};

typedef struct {
	LoxObj obj;
	/** same as return value of strlen */
	size_t length;
	char chars[];
} LoxString;

void printObject(Value value);

static inline bool isObjType(Value value, ObjType type) {
	return IS_OBJ(value) && AS_OBJ(value)->type == type;
}
