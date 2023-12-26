CC=cc
CFLAGS= -g -Wall -Wextra -Wpedantic -Werror -Wno-gnu-statement-expression
SRC_FILES= main.c chunk.c debug.c memory.c stack.c value.c vm.c

lox: main.c chunk.c debug.c memory.c stack.c value.c vm.c
	$(CC) $(CFLAGS) -D DEBUG_TRACE_EXECUTION $(SRC_FILES) -o lox
test: test-stack test-vm
test-vm: test/test-vm.c vm.h vm.c chunk.h chunk.c
	$(CC) $(CFLAGS) test/test-vm.c vm.c chunk.c stack.c memory.c value.c debug.c -g -o test/test-vm
	test/test-vm
test-stack: test/test-stack.c stack.h stack.c
	$(CC) $(CFLAGS) test/test-stack.c stack.c memory.c -g -o test/test-stack
	test/test-stack

clangd:
	clang $(CFLAGS) -MJ compile_commands.json $(SRC_FILES) -o lox
