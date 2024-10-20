#include <stdio.h>
#include <string.h>
int main(int argc, char* argv[]) {
	char const* strLiteral = "string literal";
	printf("length of %s: %lu\n", strLiteral, strlen(strLiteral));
	char* emptyString = "";
	printf("length of %s: %lu\n", emptyString, strlen(emptyString));
	char* stringWithNull = "\0";
	printf("length of %s: %lu\n", stringWithNull, strlen(stringWithNull));

	printf("length of %s: %lu\n", argv[1], strlen(argv[1]));
}
