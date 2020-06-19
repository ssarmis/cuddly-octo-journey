#pragma once 

#include "window.h"
#include "panel.h"
#include "gap_buffer.h"

void gotoLineAction(void* data0, void* data1){
    EditorWindow* window = (EditorWindow*)data0;
    GapBuffer* lineBuffer = (GapBuffer*)data1;

    char* lineString = gapToString(lineBuffer);
    u32 line = readU32FromBuffer((u8*)lineString);

    gapSeekCursorToLine(&window->buffer, line);
    TRACE("Got to line: %d\n", line);

    // clean panel gap buffer
}

void openFileAction(void* data0, void* data1){
    EditorWindow* window = (EditorWindow*)data0;
    GapBuffer* filenameBuffer = (GapBuffer*)data1;
    char* filename = gapToString(filenameBuffer);

    if(window->buffer.filename){
        // TODO(Sarmis) add some save thing
        // even something like a logo or idk
        // to indicate that someting was saved
        // and make it global so it can be called from anywhere
        gapWriteFile(&window->buffer);
        TRACE("Saved file %s\n", window->buffer.filename);
    }

    window->buffer = gapReadFile(filename);

    window->scrollX = 0;
    window->scrollY = 0;

    // clean panel gap buffer
}