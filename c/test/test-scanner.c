#include "../lox_assert.h"
#include "../scanner.h"

static void eof() {
	char const* src = "\0";
	Scanner scanner;
	initScanner(&scanner, src);
	Token t = scanToken(&scanner);
	LOX_ASSERT(t.type == TOKEN_EOF);
}

static void single_character_tokens() {
	Scanner scanner;

	char const* src = "(){};,.+/*";
	initScanner(&scanner, src);

	TokenType wants[] = {TOKEN_LEFT_PAREN,	TOKEN_RIGHT_PAREN, TOKEN_LEFT_BRACE,
						 TOKEN_RIGHT_BRACE, TOKEN_SEMICOLON,   TOKEN_COMMA,
						 TOKEN_DOT,			TOKEN_PLUS,		   TOKEN_SLASH,
						 TOKEN_STAR};

	for (unsigned long i = 0; i < sizeof(wants) / sizeof(wants[0]); ++i) {
		Token t = scanToken(&scanner);
		TokenType want = wants[i];
		TokenType got = t.type;
		if (got != want) {
			fflush(stdout);
			fprintf(
				stderr, "wrong token type.  Expected '%c' to be %s, got %s\n",
				*t.start, tokenTypeToString_(want), tokenTypeToString_(got));
			LOX_ASSERT(1 == 0);
		}
	}
	Token t = scanToken(&scanner);
	TokenType want = TOKEN_EOF;
	TokenType got = t.type;
	LOX_ASSERT_EQUALS(got, want)
}

static void skip_whitespace() {
	Scanner scanner;
	char const* src = "(	{\r,  +\n; ";
	initScanner(&scanner, src);

	TokenType wants[] = {
		TOKEN_LEFT_PAREN, TOKEN_LEFT_BRACE, TOKEN_COMMA,
		TOKEN_PLUS,		  TOKEN_SEMICOLON,
	};

	for (unsigned long i = 0; i < sizeof(wants) / sizeof(wants[0]); ++i) {
		Token t = scanToken(&scanner);
		TokenType want = wants[i];
		TokenType got = t.type;
		if (got != want) {
			fflush(stdout);
			fprintf(
				stderr, "wrong token type.  Expected '%c' to be %s, got %s\n",
				*t.start, tokenTypeToString_(want), tokenTypeToString_(got));
			LOX_ASSERT(1 == 0);
		}
	}
	/* LOX_ASSERT_EQUALS(scanToken(&scanner).type, TOKEN_EOF); */
	/* LOX_ASSERT_EQUALS(scanner.line, 2); */
}

int main(void) {
	eof();
	single_character_tokens();
	skip_whitespace();
}
