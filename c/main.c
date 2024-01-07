#include <stdio.h>

#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "lox_assert.h"
#include "value.h"
#include "vm.h"


static void runFile(char* filename) {
	LOX_ASSERT(false && filename && "unimplemented");
}

static void repl(void) {
	char line[1024];
	while (true) {
		printf("> ");
		if (!fgets(line, sizeof(line), stdin)) {
			printf("\n");
			break;
		}
		interpret(line);
	}
}
int main(int argc, char* argv[]) {
	if (argc == 1) {
		repl();
	} else if (argc == 2) {
		runFile(argv[1]);
	} else {
		fprintf(stderr, "Usage: clox [path]\n");
	}
	return 0;
}
