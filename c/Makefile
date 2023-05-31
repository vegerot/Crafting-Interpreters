lox: main.c chunk.c debug.c memory.c stack.c value.c vm.c
	cc -g -Wall -Wextra -Wpedantic -Werror -Wno-gnu-statement-expression main.c chunk.c debug.c memory.c stack.c value.c vm.c -o lox
test-stack: test/test-stack.c stack.h stack.c
	cc -Wall -Wextra -Wpedantic -Werror -Wno-gnu-statement-expression test/test-stack.c stack.c memory.c -g -o test/test
	test/test

