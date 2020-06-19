#pragma once

#include "general.h"

#include "math.h"

v3 rgbToNormalized(u8 r, u8 g, u8 b){
    v3 result = {};

    result.x = (r32)(r) / 255.0;
    result.y = (r32)(g) / 255.0;
    result.z = (r32)(b) / 255.0;

    return result;
}

#if 1

v3 DEFAULT_COLOR_BACKGROUND = {0.1, 0.1, 0.1};
v3 DEFAULT_COLOR_TEXT = {1, 1, 1};
v3 SELECTION_COLOR_TEXT = {1, 0, 0};
v3 STRING_COLOR_TEXT = {1, 0.5, 0.5};

v3 KEYWORD_COLOR_TYPE = {0, 0.5, 1};
v3 KEYWORD_COLOR_SPECIAL = KEYWORD_COLOR_TYPE;

#else
v3 DEFAULT_COLOR_BACKGROUND = rgbToNormalized(0x85, 0xdc, 0xb0);
v3 DEFAULT_COLOR_TEXT = rgbToNormalized(0xee, 0xee, 0xee);
v3 SELECTION_COLOR_TEXT = rgbToNormalized(0xc3, 0x8d, 0x9e);
v3 STRING_COLOR_TEXT = rgbToNormalized(0xc3, 0x8d, 0x9e);

v3 KEYWORD_COLOR_TYPE = rgbToNormalized(0xe2, 0x7d, 0x60);
v3 KEYWORD_COLOR_SPECIAL = KEYWORD_COLOR_TYPE;
#endif