#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lox_assert.h"
#include "memory.h"

void* reallocate(void* pointer, size_t newSize) {
	if (newSize == 0) {
		free(pointer);
		return NULL;
	}

	void* result = realloc(pointer, newSize);
	if (result == NULL) {
		fprintf(stderr, "out of memory.  You've fucked up: %s",
				strerror(errno));
		exit(1);
	}
	return result;
}

static void freeLoxObject(LoxObj* obj) {
	switch (obj->type) {
	case OBJ_STRING: {
		// I don't need to free the chars separately since the chars are inside
		// the LoxString struct
		// LoxString* string = (LoxString*)obj;
		// free(string->chars);
		free(obj);
		break;
	}
	default:
		LOX_ASSERT(false && "unknown object type")
	}
}

void freeObjects(VM* vm) {
#ifdef DEBUG_TRACE_EXECUTION
	printf("freeing objects\n");
#endif
	LoxObj* curr = vm->objects;
	LoxObj* next = NULL;
	while (curr) {
		next = curr->next;
		freeLoxObject(curr);
		curr = next;
	}
}
