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
LoxString* newLoxStringFromCStringAndHash(VM* vm, char const* cString,
										  size_t length, uint32_t hash);

#include "../src/memory.h"
#include "../src/value.h"

typedef enum { ENTRY_ERR, ENTRY_EMPTY, ENTRY_VALUE, ENTRY_TOMBSTONE } EntryType;
typedef struct {
	EntryType type;
	LoxString* key;
	Value value;
} TableEntry;

typedef struct {
	/** number of non-ENTRY_EMPTY entries */
	size_t count;
	size_t capacity;
	TableEntry* entries;
} LoxTable;

void tableInit(LoxTable* this, size_t initialCapacity);
bool tableSet(LoxTable* table, LoxString* key, Value value);
void tableAddAll(LoxTable* from, LoxTable* to);
static void adjustCapacity(LoxTable* this, size_t newCapacity) {
	TableEntry* newEntries = ALLOCATE(TableEntry, newCapacity);

	// init each entry
	for (size_t i = 0; i < newCapacity; ++i) {
		newEntries[i] =
			(TableEntry){.type = ENTRY_EMPTY, .key = NULL, .value = NIL_VAL};
	}
	LoxTable newThis = {
		.capacity = newCapacity,
		/** set in `tableAddAll */
		.count = 0,
		.entries = newEntries,
	};

	bool isEntriesUninitialized = this->entries == NULL;
	if (!isEntriesUninitialized) {
		tableAddAll(this, &newThis);
		free(this->entries);
	}

	*this = newThis;
}

void tableInit(LoxTable* this, size_t initialCapacity) {
	/** this->capacity is set in `adjustCapacity` */
	this->capacity = 0;
	this->count = 0;
	this->entries = NULL;
	if (initialCapacity > 0) {
		adjustCapacity(this, initialCapacity);
	}
}
void tableFree(LoxTable* table);
void tableFree(LoxTable* table) {
	if (table->entries == NULL) {
		LOX_ASSERT(false && "trying to free uninitialized table");
	}
	FREE_ARRAY(Entry, table->entries, table->capacity);
}

static TableEntry* findEntry(LoxTable const* this, LoxString const* key) {
	if (this->capacity == 0)
		LOX_ASSERT(false && "attempting to find entry in uninitialized table");
	uint32_t index = key->hash % this->capacity;
	TableEntry* lastTombstone = NULL;
	while (true) {
		TableEntry* curr = &this->entries[index];
		if (curr->type == ENTRY_VALUE && curr->key->hash == key->hash)
			return curr;
		if (curr->type == ENTRY_TOMBSTONE)
			lastTombstone = curr;
		if (curr->type == ENTRY_EMPTY)
			return lastTombstone == NULL ? curr : lastTombstone;

		index = (index + 1) % (this->capacity);
	}
}

void tableAddAll(LoxTable* from, LoxTable* to) {
	if (from->count == 0)
		return;
	for (size_t i = 0; i < from->capacity; ++i) {
		TableEntry* entry = &from->entries[i];
		if (entry->type == ENTRY_VALUE) {
			++to->count;
			tableSet(to, entry->key, entry->value);
		}
	}
}

#define TABLE_MAX_LOAD_RATIO 0.69
static_assert((int)TABLE_MAX_LOAD_RATIO == 0,
			  "TABLE_MAX_LOAD_RATIO must be less than 1");
bool tableSet(LoxTable* this, LoxString* key, Value value) {
	// FIXME: this conversion looks weird.  Probably should multiply and divide
	// whole numbers instead
	if (this->count + 1 >
		(size_t)((int)this->capacity * TABLE_MAX_LOAD_RATIO)) {
		size_t newCapacity = GROW_CAPACITY(this->capacity);
		adjustCapacity(this, newCapacity);
	}
	TableEntry* entry = findEntry(this, key);
	if (entry->type == ENTRY_EMPTY)
		++this->count;

	entry->type = ENTRY_VALUE;
	entry->key = key;
	entry->value = value;
	return true;
}
Value* tableGet(LoxTable const* table, LoxString* key);
Value* tableGet(LoxTable const* this, LoxString* key) {
	if (this->count == 0)
		return NULL;
	TableEntry* entry = findEntry(this, key);
	if (entry->type != ENTRY_VALUE) {
		return NULL;
	}
	return &entry->value;
}

bool tableDelete(LoxTable* this, LoxString const* key);
bool tableDelete(LoxTable* this, LoxString const* key) {
	if (this->count == 0)
		return false;
	TableEntry* entry = findEntry(this, key);
	if (entry->type != ENTRY_VALUE)
		return false;
	*entry =
		(TableEntry){.type = ENTRY_TOMBSTONE, .key = NULL, .value = NIL_VAL};
	--this->count;
	return true;
}

// -----------------------

static void basic(void) {
	initVM();
	VM* vm = getVM_();
	LoxTable table;
	tableInit(&table, 0);

	LOX_ASSERT_EQUALS(table.count, 0);

	char const* k1 = "key1";
	LoxString* key1 = newLoxStringFromCString(vm, k1, strlen(k1));
	Value one = NUMBER_VAL(1);
	tableSet(&table, key1, one);

	LOX_ASSERT_VALUE_EQUALS(*tableGet(&table, key1), one);
	LOX_ASSERT_EQUALS(table.count, 1);

	char const* k2 = "key2";
	LoxString* key2 = newLoxStringFromCString(vm, k2, strlen(k2));
	Value two = NUMBER_VAL(2);
	tableSet(&table, key2, two);

	LOX_ASSERT_VALUE_EQUALS(*tableGet(&table, key1), one);
	LOX_ASSERT_VALUE_EQUALS(*tableGet(&table, key2), two);
	LOX_ASSERT_EQUALS(table.count, 2);

	tableFree(&table);
	freeVM();
}

static void overwrite(void) {
	initVM();
	VM* vm = getVM_();
	LoxTable table;
	tableInit(&table, 0);
	char const* k = "key";
	LoxString* key = newLoxStringFromCString(vm, "key", strlen(k));
	Value one = NUMBER_VAL(1);
	tableSet(&table, key, one);

	Value* res = tableGet(&table, key);

	LOX_ASSERT_VALUE_EQUALS(*res, one);
	LOX_ASSERT_EQUALS(table.count, 1);
	Value want = NUMBER_VAL(2);
	tableSet(&table, key, want);

	res = tableGet(&table, key);

	LOX_ASSERT_VALUE_EQUALS(*res, want);
	LOX_ASSERT_EQUALS(table.count, 1);
	tableFree(&table);
	freeVM();
}

static void delete(void) {
	initVM();
	VM* vm = getVM_();
	LoxTable table;
	tableInit(&table, 8);
	char const* k1 = "key1";
	uint32_t hash1 = table.capacity * 1;
	LoxString* key1 = newLoxStringFromCStringAndHash(vm, k1, strlen(k1), hash1);
	Value one = NUMBER_VAL(1);
	tableSet(&table, key1, one);

	char const* k2 = "key2";
	uint32_t hash2 = table.capacity * 2;
	LoxString* key2 = newLoxStringFromCStringAndHash(vm, k2, strlen(k2), hash2);
	Value two = NUMBER_VAL(2);
	tableSet(&table, key2, two);

	LOX_ASSERT_EQUALS(table.count, 2);
	LOX_ASSERT_VALUE_EQUALS(*tableGet(&table, key1), one);
	LOX_ASSERT_EQUALS(tableDelete(&table, key1), true)
	LOX_ASSERT(tableGet(&table, key1) == NULL);
	LOX_ASSERT_EQUALS(tableDelete(&table, key1), false)
	LOX_ASSERT_EQUALS(table.count, 1);

	LOX_ASSERT_VALUE_EQUALS(*tableGet(&table, key2), two);
	LOX_ASSERT_EQUALS(tableDelete(&table, key2), true)
	LOX_ASSERT(tableGet(&table, key2) == NULL);
	LOX_ASSERT_EQUALS(tableDelete(&table, key2), false)
	LOX_ASSERT_EQUALS(table.count, 0);

	tableFree(&table);
	freeVM();
}
static void deletenoinfiniteloop(void) {
	initVM();
	VM* vm = getVM_();
	LoxTable table;
	tableInit(&table, 2);
	char const* k1 = "key1";
	uint32_t hash1 = table.capacity * 1;
	LoxString* key1 = newLoxStringFromCStringAndHash(vm, k1, strlen(k1), hash1);
	Value one = NUMBER_VAL(1);
	tableSet(&table, key1, one);

	char const* k2 = "key2";
	uint32_t hash2 = table.capacity * 2;
	LoxString* key2 = newLoxStringFromCStringAndHash(vm, k2, strlen(k2), hash2);
	Value two = NUMBER_VAL(2);
	tableSet(&table, key2, two);

	LOX_ASSERT_VALUE_EQUALS(*tableGet(&table, key1), one);
	LOX_ASSERT_EQUALS(tableDelete(&table, key1), true)
	LOX_ASSERT(tableGet(&table, key1) == NULL);
	LOX_ASSERT_EQUALS(tableDelete(&table, key1), false)

	LOX_ASSERT_VALUE_EQUALS(*tableGet(&table, key2), two);
	LOX_ASSERT_EQUALS(tableDelete(&table, key2), true)
	LOX_ASSERT(tableGet(&table, key1) == NULL);
	LOX_ASSERT_EQUALS(tableDelete(&table, key2), false)

	char const* k3 = "key3";
	uint32_t hash3 = table.capacity * 3;
	LoxString* key3 = newLoxStringFromCStringAndHash(vm, k3, strlen(k3), hash3);
	findEntry(&table, key3);

	tableFree(&table);
	freeVM();
}

static void grow(void) {
	initVM();
	VM* vm = getVM_();
	LoxTable table;
	tableInit(&table, 1);
	LOX_ASSERT_EQUALS(table.capacity, 1);

	char const* k = "key";
	LoxString* key = newLoxStringFromCString(vm, "key", strlen(k));
	Value one = NUMBER_VAL(1);
	tableSet(&table, key, one);

	Value* res = tableGet(&table, key);
	LOX_ASSERT_VALUE_EQUALS(*res, one);
	LOX_ASSERT_EQUALS(table.capacity, 8);

	tableFree(&table);
	freeVM();
}

/**
 * This tests the Open Addressing behavior of the hash table, i.e. the behavior
 * when two keys are mapped to the same offset in the table.
 *
 * Problem: I don't have a direct way of testing it.
 * Testing strategy: fill the table with a bunch of keys, and _surely_ at some
 * point there will be a collision
 */
static void collision(void) {
	initVM();
	VM* vm = getVM_();
	LoxTable table;
	tableInit(&table, 0);

	// fill table with a bunch of keys.  Assume there are some collisions...
	for (char c = 'A'; c < (char)'z'; ++c) {
		char const k[] = {c, '\0'};
		LoxString* key = newLoxStringFromCString(vm, k, strlen(k));
		Value val = NUMBER_VAL(c);
		tableSet(&table, key, val);
	}

	// check everything is in the table
	for (char c = 'A'; c < (char)'z'; ++c) {
		char const k[] = {c, '\0'};
		LoxString* key = newLoxStringFromCString(vm, k, strlen(k));
		Value lol = NUMBER_VAL(c);
		Value* have = tableGet(&table, key);
		LOX_ASSERT_VALUE_EQUALS(*have, lol);
	}

	tableFree(&table);
	freeVM();
}

int main(void) {
	basic();
	overwrite();
	delete ();
	deletenoinfiniteloop();
	grow();

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
