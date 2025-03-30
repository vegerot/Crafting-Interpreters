#include "table.h"
#include "memory.h"
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
		bool is_same_string =
			(key == curr->key) ||
			(curr->type == ENTRY_VALUE && curr->key->hash == key->hash);
		if (is_same_string)
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

LoxString* tableFindString(LoxTable* table, char const* chars, size_t length,
						   uint32_t hash) {
	if (table->count == 0)
		return NULL;

	uint32_t index = hash % table->capacity;
	while (true) {
		TableEntry* entry = &table->entries[index];
		bool is_match = entry->key != NULL && entry->key->length == length &&
						entry->key->hash == hash &&
						memcmp(entry->key->chars, chars, length) == 0;
		if (entry->key == NULL && IS_NIL(entry->value)) {
			return NULL;

		} else if (is_match) {
			return entry->key;
		}

		index = (index + 1) % table->capacity;
	}
}

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

Value* tableGet(LoxTable const* this, LoxString* key) {
	if (this->count == 0)
		return NULL;
	TableEntry* entry = findEntry(this, key);
	if (entry->type != ENTRY_VALUE) {
		return NULL;
	}
	return &entry->value;
}

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
