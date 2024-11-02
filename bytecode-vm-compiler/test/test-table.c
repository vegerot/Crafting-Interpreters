#include "../src/lox_assert.h"
#include "../src/vm.h"
#include <string.h>
LoxString* allocateString(VM* vm, char const* cString, size_t length);

#include "../src/common.h"
#include "../src/memory.h"
#include "../src/value.h"

typedef struct {
	LoxString* key;
	Value value;
} TableEntry;

typedef struct {
	size_t count;
	size_t capacity;
	TableEntry* entries;
} LoxTable;

void tableInit(LoxTable* table);
void tableInit(LoxTable* table) {
	table->count = 0;
	table->capacity = table->count;
	table->entries = NULL;
}
void tableFree(LoxTable* table);
void tableFree(LoxTable* table) {
	if (table->entries == NULL) {
		LOX_ASSERT(false && "trying to free uninitialized table");
	}
	FREE_ARRAY(Entry, table->entries, table->capacity);
}
bool tableSet(LoxTable* table, LoxString* key, Value value);
bool tableSet(LoxTable* table, LoxString* key, Value value) {}
Value* tableGet(LoxTable* table, LoxString* key);
Value* tableGet(LoxTable* table, LoxString* key) {}

int main(void) {
	// basic
	initVM();
	VM vm = getVM_();
	LoxTable table;
	tableInit(&table);
	char const* k = "key";
	LoxString* key = allocateString(&vm, "key", strlen(k));
	Value want = NUMBER_VAL(42.0);
	tableSet(&table, key, want);

	Value* res = tableGet(&table, key);

	LOX_ASSERT_VALUE_EQUALS(*res, want);
	tableFree(&table);
}
