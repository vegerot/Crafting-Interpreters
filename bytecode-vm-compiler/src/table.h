/**
 * idea: make this its own library independent of Lox, with keys of type
 * `{uint32_t hash}` and values of type `void*`.
 * Then create a wrapper in Lox that takes keys with type `LoxString` and values
 * of type `Value*`
 */
#pragma once
#include <string.h>

#include "lox_assert.h"
#include "object.h"
#include "value.h"

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
LoxString* tableFindString(LoxTable* table, char const* chars, size_t length,
						   uint32_t hash);
void tableFree(LoxTable* table);

#define TABLE_MAX_LOAD_RATIO 0.69
static_assert((int)TABLE_MAX_LOAD_RATIO == 0,
			  "TABLE_MAX_LOAD_RATIO must be less than 1");
Value* tableGet(LoxTable const* table, LoxString* key);

bool tableDelete(LoxTable* this, LoxString const* key);
