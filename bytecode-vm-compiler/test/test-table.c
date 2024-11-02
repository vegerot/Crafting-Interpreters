/**
 * idea: make this its own library independent of Lox, with keys of type
 * `{uint32_t hash}` and values of type `void*`.
 * Then create a wrapper in Lox that takes keys with type `LoxString` and values
 * of type `Value*`
 */
#include "../src/lox_assert.h"
#include "../src/vm.h"
#include <string.h>
LoxString* newLoxStringFromCString(VM* vm, char const* cString, size_t length);

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

static TableEntry* findEntry(LoxTable const* table, LoxString const* key) {
	uint32_t index = key->hash % table->capacity;
	while (true) {
		// printf("index: %u\n", index);
		TableEntry* curr = &table->entries[index];
		if (curr->key == NULL || curr->key->hash == key->hash)
			return curr;
		index = (index + 1) % (table->capacity);
	}
}

static void adjustCapacity(LoxTable* table, size_t newCapacity) {
	TableEntry* newEntries = ALLOCATE(TableEntry, newCapacity);

	// init each entry
	for (size_t i = 0; i < newCapacity; ++i) {
		newEntries[i].key = NULL;
		newEntries[i].value = NIL_VAL;
	}

	table->capacity = newCapacity;
	table->entries = newEntries;
}

bool tableSet(LoxTable* table, LoxString* key, Value value);
#define TABLE_MAX_LOAD_RATIO 0.69
bool tableSet(LoxTable* table, LoxString* key, Value value) {
	// FIXME: this conversion looks weird.  Probably should multiply and divide
	// whole numbers instead
	if (table->count + 1 >
		(size_t)((int)table->capacity * TABLE_MAX_LOAD_RATIO)) {
		size_t newCapacity = GROW_CAPACITY(table->capacity);
		adjustCapacity(table, newCapacity);
	}
	TableEntry* entry = findEntry(table, key);
	++table->count;

	entry->key = key;
	entry->value = value;
	return true;
}
Value* tableGet(LoxTable* table, LoxString* key);
Value* tableGet(LoxTable* table, LoxString* key) {
	return &findEntry(table, key)->value;
}

static void basic(void) {
	initVM();
	VM* vm = getVM_();
	LoxTable table;
	tableInit(&table);
	char const* k = "key";
	LoxString* key = newLoxStringFromCString(vm, "key", strlen(k));
	Value want = NUMBER_VAL(42.0);
	tableSet(&table, key, want);

	Value* res = tableGet(&table, key);

	LOX_ASSERT_VALUE_EQUALS(*res, want);
	tableFree(&table);
	freeVM();
}

static void overwrite(void) {
	initVM();
	VM* vm = getVM_();
	LoxTable table;
	tableInit(&table);
	char const* k = "key";
	LoxString* key = newLoxStringFromCString(vm, "key", strlen(k));
	Value one = NUMBER_VAL(1);
	tableSet(&table, key, one);

	Value* res = tableGet(&table, key);

	LOX_ASSERT_VALUE_EQUALS(*res, one);
	Value want = NUMBER_VAL(2);
	tableSet(&table, key, want);

	res = tableGet(&table, key);

	LOX_ASSERT_VALUE_EQUALS(*res, want);
	tableFree(&table);
	freeVM();
}

/**
 * This test no longer does what it was designed for.
 * When I wrote this test, the hash function was hardcoded to always return 69,
 * and this test passed.  Now we have a better hash function.
 */
static void collision(void) {
	initVM();
	VM* vm = getVM_();
	LoxTable table;
	tableInit(&table);
	char const* k1 = "costarring";
	LoxString* key1 = newLoxStringFromCString(vm, k1, strlen(k1));
	Value one = NUMBER_VAL(1);
	tableSet(&table, key1, one);

	char const* k2 = "liquid";
	LoxString* key2 = newLoxStringFromCString(vm, k2, strlen(k2));
	Value two = NUMBER_VAL(2);
	tableSet(&table, key2, two);

	Value* res1 = tableGet(&table, key1);
	Value* res2 = tableGet(&table, key2);

	LOX_ASSERT_VALUE_EQUALS(*res1, one);
	LOX_ASSERT_VALUE_EQUALS(*res2, two);
	tableFree(&table);
	freeVM();
}

int main(void) {
	basic();
	overwrite();
	// delete();
	// grow();

	// FIXME: how do I test collisions?
	// ideas:
	// 1. take the hash function as an argument
	// 2. make a small hash table, and add hundreds of entries to it,
	// statistically there'd be a collision somewhere in here...
	// I prefer 2.
	//
	// Currently, when I want to test this function, I change the
	// `computeHashOfCString` function to always return 69.  That works too...
	collision();
}
