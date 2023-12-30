#ifndef lox_assert_h
#define lox_assert_h
#include <stdio.h>
#include <stdlib.h>
/// could try `__builtin_expect` if this is too slow
#define LOX_ASSERT(cond)                                                       \
	(!(cond) ? LOX_ASSERT_FAIL(__func__, __FILE__, __LINE__, #cond) : (void)0);

#define LOX_ASSERT_FAIL(func, file, line, cond_literal)                        \
                                                                               \
	__extension__({                                                            \
		fflush(stdout);                                                        \
		fprintf(stderr, "\n%s:%d: %s: Assertion `%s` failed.\n", file, line,   \
				func, cond_literal);                                           \
		abort();                                                               \
	})
#endif
