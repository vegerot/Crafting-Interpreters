#include "../lox_assert.h"
#include "../scanner.h"
#include <string.h>

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

static void multi_character_tokens() {
	Scanner scanner;

	char const* src = "!;!=;<;<=;>;>=";
	initScanner(&scanner, src);

	TokenType wants[] = {
		TOKEN_BANG,	   TOKEN_SEMICOLON, TOKEN_BANG_EQUAL,	 TOKEN_SEMICOLON,
		TOKEN_LESS,	   TOKEN_SEMICOLON, TOKEN_LESS_EQUAL,	 TOKEN_SEMICOLON,
		TOKEN_GREATER, TOKEN_SEMICOLON, TOKEN_GREATER_EQUAL,
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
	LOX_ASSERT_EQUALS(scanToken(&scanner).type, TOKEN_EOF);
	LOX_ASSERT_EQUALS(scanner.line, 2);
}

static void skip_comments() {
	Scanner scanner;
	char const* src = "//asdf roflmao\n(	{\r,  +\n;";
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
	LOX_ASSERT_EQUALS(scanToken(&scanner).type, TOKEN_EOF);
	LOX_ASSERT_EQUALS(scanner.line, 3);
}

static void scan_strings() {
	Scanner scanner;
	char const* src = "(\"hello\nworld!\")";
	initScanner(&scanner, src);

	Token t = scanToken(&scanner);
	LOX_ASSERT_EQUALS(t.type, TOKEN_LEFT_PAREN)

	t = scanToken(&scanner);
	LOX_ASSERT_EQUALS(t.type, TOKEN_STRING);
	LOX_ASSERT_EQUALS(t.line, 2);
	LOX_ASSERT_EQUALS(t.length, (int)strlen("\"hello world!\""));
	int equal = strncmp(t.start, "\"hello\nworld!\"", t.length);
	LOX_ASSERT(equal == 0);

	t = scanToken(&scanner);
	LOX_ASSERT_EQUALS(t.type, TOKEN_RIGHT_PAREN);
}

static void unterminated_string() {
	Scanner scanner;
	char const* src = "(\"hello world!)";
	initScanner(&scanner, src);

	Token t = scanToken(&scanner);
	LOX_ASSERT_EQUALS(t.type, TOKEN_LEFT_PAREN)

	t = scanToken(&scanner);
	LOX_ASSERT_EQUALS(t.type, TOKEN_ERROR);
	LOX_ASSERT_EQUALS(t.length, (int)strlen("\"hello world!)"));
	LOX_ASSERT_EQUALS(t.error_message_length,
					  (int)strlen("Unterminated string"));
	int equal =
		strncmp(t.error_message, "Unterminated string", t.error_message_length);
	LOX_ASSERT(equal == 0);
}

int main(void) {
	eof();
	single_character_tokens();
	multi_character_tokens();
	skip_whitespace();
	skip_comments();
	scan_strings();
	unterminated_string();
}
