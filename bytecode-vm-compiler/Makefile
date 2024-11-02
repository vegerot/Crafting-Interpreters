SHELL=/bin/bash
.SHELLFLAGS := -o pipefail -o errexit -o nounset -c

CC ?= cc
CFLAGS ?= -std=gnu2x -g -Wall -Wextra -Wpedantic -Wimplicit-fallthrough -Werror -Wno-gnu-statement-expression -Wno-strict-prototypes -Wno-unused-parameter
SRC_FILES= src/chunk.c src/compiler.c src/debug.c src/memory.c src/object.c src/scanner.c src/stack.c src/value.c src/vm.c

lox: src/main.c $(SRC_FILES) src/*.h
	$(CC) $(CFLAGS) -DDEBUG_TRACE_EXECUTION -DDEBUG_PRINT_CODE -fsanitize=address src/main.c $(SRC_FILES) -o lox

all: format lint ci
ci: lox test clangd lint-check # do not run format-check because GH A uses an incompatible version of clang-format

test: test/test-stack test/test-vm test/test-scanner test/test-compiler test/test-table
test/test-vm: test/test-vm.c src/vm.h src/vm.c src/chunk.h src/chunk.c src/value.h src/value.c src/compiler.h src/compiler.c
	$(CC) -fsanitize=address $(CFLAGS)  -Wno-error test/test-vm.c $(SRC_FILES) -o test/test-vm
	test/test-vm || (mv test/test-vm test/test-vm.failed && false)
test/test-stack: test/test-stack.c src/stack.h src/stack.c
	$(CC) -fsanitize=address $(CFLAGS) -Wno-error test/test-stack.c $(SRC_FILES) -o test/test-stack
	test/test-stack
test/test-scanner: test/test-scanner.c src/scanner.h src/scanner.c src/lox_assert.h
	$(CC) -fsanitize=address $(CFLAGS)  -Wno-error test/test-scanner.c $(SRC_FILES) -o test/test-scanner
	test/test-scanner || mv test/test-scanner test/test-scanner.failed
test/test-compiler: test/test-compiler.c src/compiler.h src/compiler.c src/scanner.h src/scanner.c src/chunk.h src/chunk.c src/value.h src/value.c
	$(CC) -DDEBUG_TRACE_EXECUTION -fsanitize=address $(CFLAGS)  -Wno-error test/test-compiler.c $(SRC_FILES) -o test/test-compiler
	test/test-compiler || mv -v test/test-compiler test/test-compiler.failed
test/test-table: test/test-table.c test/test-vm
	$(CC) $(CFLAGS)  -fsanitize=address $(EXTRA_CFLAGS) -Wno-error test/test-table.c $(SRC_FILES) -o test/test-table
	test/test-table || mv test/test-table test/test-table.failed


# NOTE: if you're seeing a bunch of errors about unknown clang flags, make
# sure `make clangd` uses the same llvm version as `make lint`
lint: compile_commands.json .lint-stamp
.lint-stamp: src/main.c $(SRC_FILES) src/*.h test/*.c .clang-tidy
	clang-tidy --warnings-as-errors='*' --fix-errors --checks="-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling" src/main.c $(SRC_FILES) src/*.h test/*.c
	touch .lint-stamp
	touch .lint-check-stamp

lint-check: compile_commands.json .lint-check-stamp
.lint-check-stamp: src/main.c $(SRC_FILES) src/*.h test/*.c .clang-tidy
	clang-tidy --warnings-as-errors='*' --checks="-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling" src/main.c $(SRC_FILES) src/*.h test/*.c
	touch .lint-check-stamp

CLANG_FORMAT ?= clang-format
format: src/main.c $(SRC_FILES) src/*.h test/*.c .clang-format
	$(CLANG_FORMAT) --Werror -i src/main.c $(SRC_FILES) src/*.h test/*.c
format-check: src/main.c $(SRC_FILES) src/*.h test/*.c .clang-format
	$(CLANG_FORMAT) --Werror -n src/main.c $(SRC_FILES) src/*.h test/*.c

CLANG ?= clang
clangd: compile_commands.json
compile_commands.json: src/main.c $(SRC_FILES) src/*.h
	printf "[" > compile_commands.json
	$(CLANG) $(CFLAGS) -Wno-error -MJ /dev/stdout -DDEBUG_TRACE_EXECUTION src/main.c $(SRC_FILES) -o lox | sed '$$s|.$$||' >> compile_commands.json
	printf "]" >> compile_commands.json

