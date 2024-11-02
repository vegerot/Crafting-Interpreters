#include "../src/vm.h"
#include <string.h>
LoxString* allocateString(VM* vm, char const* cString, int length);
int main(void) {
	// basic
	initVM();
	VM vm = getVM_();
	LoxTable table;
	tableInit(&table);
	char const* k = "key";
	LoxString* key = allocateString(&vm, "key", strlen(k));
	tableSet(&table, key, NUMBER_VAL(42.0));

}
