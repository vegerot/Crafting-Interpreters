#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* hello(char const* name, int nameLen) {
  char* format = "Hello, %s!\n";
  // larger than required because of format specifiers
  int lengthOfTarget = strlen(format) + nameLen;
  char* ret = malloc(sizeof(char) * lengthOfTarget);
  snprintf(ret, lengthOfTarget, format, name);
  return ret;
}

void shouldPrintHelloMax() {
  char* hiMax = hello("Max", 3);
  assert(strcmp(hiMax, "Hello, Max!\n") == 0);
  free(hiMax);
}

void test() { shouldPrintHelloMax(); }

int main() { test(); }
