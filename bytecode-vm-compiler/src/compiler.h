#pragma once

#include "chunk.h"
#include "object.h"
#include <stdbool.h>
bool compile(char const* source, Chunk* chunk);
void setCompilingChunk(Chunk* chunk);
LoxString* fromCString(char const* cString, int length);
