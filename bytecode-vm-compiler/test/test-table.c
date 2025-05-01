#include "../src/lox_assert.h"
#include "../src/table.h"
#include "../src/vm.h"

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
	tableGet(&table, key3);

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
