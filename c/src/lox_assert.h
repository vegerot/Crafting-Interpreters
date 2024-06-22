#ifndef lox_assert_h
#define lox_assert_h
#include <stdbool.h>
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

#define LOX_ASSERT_EQUALS(got, want)                                           \
	(((got) != (want)) ? LOX_ASSERT_EQUALS_FAIL(__func__, __FILE__, __LINE__,  \
												#got, got, #want, want)        \
					   : (void)0);
#define LOX_ASSERT_VALUE_EQUALS(g, want)                                       \
	do {                                                                       \
		Value got = g;                                                         \
		LOX_ASSERT_EQUALS(got.type, (want).type);                              \
		switch ((want).type) {                                                 \
		case (VAL_BOOL):                                                       \
			LOX_ASSERT_EQUALS(got.as.boolean, (want).as.boolean);              \
			break;                                                             \
		case (VAL_NUMBER):                                                     \
			LOX_ASSERT_EQUALS(got.as.number, (want).as.number)                 \
			break;                                                             \
		case (VAL_NIL):                                                        \
			break;                                                             \
		default:                                                               \
			LOX_ASSERT(false && "wtf?!");                                      \
		}                                                                      \
	} while (0);

#define LOX_ASSERT_EQUALS_FAIL(func, file, line, got_name, got_value,          \
							   want_name, want_value)                          \
                                                                               \
	__extension__({                                                            \
		fflush(stdout);                                                        \
		fprintf(stderr, "\n%s:%d: %s: Expected %s(=%d) to equal %s(%d)\n",     \
				file, line, func, got_name, (int)(got_value), want_name,       \
				(int)(want_value));                                            \
		abort();                                                               \
	})

#endif

#ifndef DEBUG_TRACE_EXECUTION
#define LOX_UNREACHABLE(reason)                                                \
	__extension__({                                                            \
		__builtin_unreachable();                                               \
		fflush(stdout);                                                        \
		fprintf(stderr, "\n%s:%d: %s: Unreachable code reached: %s\n",         \
				__FILE__, __LINE__, __func__, reason);                         \
		abort();                                                               \
	})
#else
#define LOX_UNREACHABLE(reason)                                                \
	__extension__({                                                            \
		fflush(stdout);                                                        \
		fprintf(stderr, "\n%s:%d: %s: Unreachable code reached: %s\n",         \
				__FILE__, __LINE__, __func__, reason);                         \
		abort();                                                               \
	})
#endif
