CC=cc
CFLAGS= -g -Wall -Wextra -Wpedantic -Werror -Wno-gnu-statement-expression
SRC_FILES= main.c chunk.c debug.c memory.c stack.c value.c vm.c

lox: main.c chunk.c debug.c memory.c stack.c value.c vm.c
	$(CC) $(CFLAGS) $(SRC_FILES) -o lox
test: test-stack
test-stack: test/test-stack.c stack.h stack.c
	$(CC) -Wall -Wextra -Wpedantic -Werror -Wno-gnu-statement-expression test/test-stack.c stack.c memory.c -g -o test/test
	test/test

clangd:
	clang $(CFLAGS) -MJ compile_commands.json $(SRC_FILES) -o lox
