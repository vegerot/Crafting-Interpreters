#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    free(pointer);
    return NULL;
  }

  void* result = realloc(pointer, newSize);
  if (result == NULL) {
    fprintf(stderr, "out of memory.  You've fucked up: %s", strerror(errno));
    exit(1);
  }
  return result;
}
