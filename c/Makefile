CC ?= cc
CFLAGS= -std=gnu2x -g -Wall -Wextra -Wpedantic -Werror -Wno-gnu-statement-expression
SRC_FILES= compiler.c scanner.c chunk.c debug.c memory.c stack.c value.c vm.c

lox: main.c chunk.c debug.c memory.c stack.c value.c vm.c
	$(CC) $(CFLAGS) -D DEBUG_TRACE_EXECUTION main.c $(SRC_FILES) -o lox

all: lox test lint
test: test-stack test-vm test-scanner
test-vm: test/test-vm.c vm.h vm.c chunk.h chunk.c
	$(CC) $(CFLAGS) test/test-vm.c $(SRC_FILES) -o test/test-vm
	test/test-vm
test-stack: test/test-stack.c stack.h stack.c
	$(CC) $(CFLAGS) test/test-stack.c $(SRC_FILES) -o test/test-stack
	test/test-stack
test-scanner: test/test-scanner.c scanner.h scanner.c
	$(CC) $(CFLAGS) test/test-scanner.c $(SRC_FILES) -o test/test-scanner
	test/test-scanner


lint: $(SRC_FILES) *.h test/*.c
	clang-tidy $(SRC_FILES) *.h test/*.c

clangd:
	printf "[" > compile_commands.json
	clang $(CFLAGS) -MJ /dev/stdout -D DEBUG_TRACE_EXECUTION main.c $(SRC_FILES) -o lox | sed '$$s|.$$||' >> compile_commands.json
	printf "]" >> compile_commands.json

