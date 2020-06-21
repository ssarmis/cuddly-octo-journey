#pragma once 

#include "window.h"
#include "panel.h"
#include "gap_buffer.h"

void gotoLineAction(void* data0, void* data1, bool* done){
    EditorWindow* window = (EditorWindow*)data0;
    GapBuffer* lineBuffer = (GapBuffer*)data1;

    char* lineString = gapToString(lineBuffer);
    u32 line = readU32FromBuffer((u8*)lineString);

    gapSeekCursorToLine(&window->buffer, line);
    TRACE("Got to line: %d\n", line);

    gapClean(lineBuffer);
    *done = true;
}

void saveFileAction(void* data0, void* data1, bool* done){
    EditorWindow* window = (EditorWindow*)data0;
    GapBuffer* filenameBuffer = (GapBuffer*)data1;
    char* filename = gapToString(filenameBuffer);

    // TODO(Sarmis) add some save thing
    // even something like a logo or idk
    // to indicate that someting was saved
    // and make it global so it can be called from anywhere
    gapWriteFile(&window->buffer, filename);
    window->background = SAVE_COLOR_BACKGROUND;
    TRACE("Saved file %s\n", window->buffer.filename);

    gapClean(filenameBuffer);
    *done = true;
}

void openFileAction(void* data0, void* data1, bool* done){
    EditorWindow* window = (EditorWindow*)data0;
    GapBuffer* filenameBuffer = (GapBuffer*)data1;
    char* filename = gapToString(filenameBuffer);

    if(window->buffer.filename){
        // TODO(Sarmis) add some save thing
        // even something like a logo or idk
        // to indicate that someting was saved
        // and make it global so it can be called from anywhere
        if(!gapWriteFile(&window->buffer)){
            *done = false;
            return;
        }
        TRACE("Saved file %s\n", window->buffer.filename);
    }

    GapBuffer buffer = gapReadFile(filename);

    if(!window->buffer.data){
        // no file could be read
        *done = false;
        return;
    }

    window->buffer = buffer;
    window->scrollX = 0;
    window->scrollY = 0;
    window->scrollTop = window->top;
    window->scrollBottom = window->bottom;

    gapClean(filenameBuffer);
    *done = true;
}