#pragma once 

#include "window.h"
#include "panel.h"
#include "gap_buffer.h"

bool findActionFromBeggining(void* data0, void* data1, void* data2){
    Panel* panel = (Panel*)data0;
    EditorWindow* window = (EditorWindow*)data1;
    GapBuffer* matchBuffer = (GapBuffer*)data2;
    char* match = gapToString(matchBuffer);

    i32 index = 0;

    // don't render old selections
    window->selections.currentAmount = 0;

    while(true){
        Selection selection = gapSeekIndexToMatch(&window->buffer, match, &index, index + 1);
        if(selection.start == selection.end){
            break;
        }
        bufferAppend<Selection>(&window->selections, selection);
    }

    return false;
}


bool findAction(void* data0, void* data1, void* data2){
    Panel* panel = (Panel*)data0;
    EditorWindow* window = (EditorWindow*)data1;
    GapBuffer* matchBuffer = (GapBuffer*)data2;
    char* match = gapToString(matchBuffer);

    Selection selection = gapSeekCursorToMatch(&window->buffer, match, panel->lastFind + 1);
    if(selection.start != selection.end){
        window->buffer.selection.start = selection.start;
        window->buffer.selection.end = selection.end;
        window->buffer.cursor = selection.end;
        window->shiftScrollToMiddle = true;

        panel->lastFind = selection.end;
    }

    return false;
}

bool saveFileAction(void* data0, void* data1, void* data2){
    Panel* panel = (Panel*)data0;
    EditorWindow* window = (EditorWindow*)data1;
    GapBuffer* filenameBuffer = (GapBuffer*)data2;
    char* filename = gapToString(filenameBuffer);

    gapWriteFile(&window->buffer, filename);
    TRACE("Saved file %s\n", window->buffer.filename);

    window->backgroundColor = {0, 0.1, 0, 1};

    return true;
}

bool gotoLineAction(void* data0, void* data1, void* data2){
    Panel* panel = (Panel*)data0;
    EditorWindow* window = (EditorWindow*)data1;
    GapBuffer* lineBuffer = (GapBuffer*)data2;

    char* lineString = gapToString(lineBuffer);
    u32 line = readU32FromBuffer((u8*)lineString);

    gapSeekCursorToLine(&window->buffer, line);
    TRACE("Got to line: %d\n", line);

    // clean panel gap buffer
    return true;
}

// add applicationlayout data or the file pool or some shit as arguments here
bool quickOpenFileAction(void* data0, void* data1, void* data2){
    Panel* panel = (Panel*)data0;
    EditorWindow* window = (EditorWindow*)data1;
    GapBuffer* filenameBuffer = (GapBuffer*)data2;
    char* filename = gapToString(filenameBuffer);

    // if(window->buffer.filename){
    //     // TODO(Sarmis) add some save thing
    //     // even something like a logo or idk
    //     // to indicate that someting was saved
    //     // and make it global so it can be called from anywhere
    //     gapWriteFile(&window->buffer);
    //     TRACE("Saved file %s\n", window->buffer.filename);
    // }

    GapBuffer buffer = gapReadFile(filename);

    if(!buffer.data){
        // clean panel buffer
        return false;
    }

    window->buffer = buffer;

    window->scrollX = 0;
    window->scrollY = 0;

    cleanStringBuffer(panel->suggestions);

    return true;
    // clean panel gap buffer
}

bool openFileAction(void* data0, void* data1, void* data2){
    Panel* panel = (Panel*)data0;
    EditorWindow* window = (EditorWindow*)data1;
    GapBuffer* filenameBuffer = (GapBuffer*)data2;
    char* filename = gapToString(filenameBuffer);

    // if(window->buffer.filename){
    //     // TODO(Sarmis) add some save thing
    //     // even something like a logo or idk
    //     // to indicate that someting was saved
    //     // and make it global so it can be called from anywhere
    //     gapWriteFile(&window->buffer);
    //     TRACE("Saved file %s\n", window->buffer.filename);
    // }

    GapBuffer buffer = gapReadFile(filename);

    if(!buffer.data){
        // clean panel buffer
        return false;
    }

    window->buffer = buffer;

    window->scrollX = 0;
    window->scrollY = 0;

    cleanStringBuffer(panel->suggestions);

    return true;
    // clean panel gap buffer
}