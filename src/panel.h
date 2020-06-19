#pragma once

#include "general.h"
#include "math.h"
#include "gap_buffer.h"

struct Panel {
    v3 position;
    v2 size;
    v3 cursor;
    char* description;
    GapBuffer buffer;
    void (*action)(void*, void*);
};

Panel panelCreate(v3 position, v2 size, char* description){
	Panel result = {};
	
	result.position = position;
	result.size = size;
	result.description = description;
	result.buffer = gapCreateEmpty();
	
    return result;
}
