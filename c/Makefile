CC ?= cc
CFLAGS= -std=gnu2x -g -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -Werror -Wno-gnu-statement-expression -Wno-strict-prototypes
SRC_FILES= src/compiler.c src/scanner.c src/chunk.c src/debug.c src/memory.c src/stack.c src/value.c src/vm.c

lox: $(SRC_FILES) src/*.h src/main.c
	$(CC) $(CFLAGS) -DDEBUG_TRACE_EXECUTION -DDEBUG_PRINT_CODE src/main.c $(SRC_FILES) -o lox

all: lox test lint
test: test/test-stack test/test-vm test/test-scanner test/test-compiler
test/test-vm: test/test-vm.c src/vm.h src/vm.c src/chunk.h src/chunk.c src/value.h src/value.c
	$(CC) $(CFLAGS) test/test-vm.c $(SRC_FILES) -o test/test-vm
	test/test-vm || mv test/test-vm test/test-vm.failed
test/test-stack: test/test-stack.c src/stack.h src/stack.c
	$(CC) $(CFLAGS) test/test-stack.c $(SRC_FILES) -o test/test-stack
	test/test-stack
test/test-scanner: test/test-scanner.c src/scanner.h src/scanner.c src/lox_assert.h
	$(CC) $(CFLAGS) -Wno-error test/test-scanner.c $(SRC_FILES) -o test/test-scanner
	test/test-scanner || mv test/test-scanner test/test-scanner.failed
test/test-compiler: test/test-compiler.c src/compiler.h src/compiler.c src/scanner.h src/scanner.c src/chunk.h src/chunk.c src/value.h src/value.c
	$(CC) $(CFLAGS) -Wno-error test/test-compiler.c $(SRC_FILES) -o test/test-compiler
	test/test-compiler || mv -v test/test-compiler test/test-compiler.failed


lint: $(SRC_FILES) src/*.h test/*.c
	clang-tidy $(SRC_FILES) src.*.h test/*.c

CLANG_FORMAT ?= clang-format
format: $(SRC_FILES) src/*.h test/*.c
	$(CLANG_FORMAT) -i $(SRC_FILES) src/*.h test/*.c

CLANG ?= clang
clangd:
	printf "[" > compile_commands.json
	$(CLANG) $(CFLAGS) -Wno-error -MJ /dev/stdout -DDEBUG_TRACE_EXECUTION src/main.c $(SRC_FILES) -o lox | sed '$$s|.$$||' >> compile_commands.json
	printf "]" >> compile_commands.json

