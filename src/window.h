#pragma once

#include "general.h"
#include "gap_buffer.h"
#include "math.h"

struct EditorWindow {
    i32 width;
    i32 height;

    r32 scrollY;
    r32 scrollX;
    r32 scrollTop;
    r32 scrollBottom;

    u32 left;
    u32 top;
    u32 bottom;

    m4 transform;
    m4 view;

    v3 cursor;

    GapBuffer buffer;
};

EditorWindow windowCreate(i32 width, i32 height, u32 left, u32 top){
    EditorWindow result = {};

    result.width = width;
    result.height = height;

    result.cursor = v3(0);

    result.left = left;
    result.top = top;
    result.bottom = result.top + height;

    result.transform = m4();
    result.view = m4();

    result.scrollX = 0;
    result.scrollY = 0;

    result.scrollTop = result.top;
    result.scrollBottom = result.bottom;

    result.buffer = gapCreateEmpty();

    return result;
}