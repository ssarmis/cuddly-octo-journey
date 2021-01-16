#pragma once 

#include "window.h"
#include "panel.h"
#include "app.h"
#include "gap_buffer.h"

bool findActionFromBeggining(void* data0){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*)data0;
    Panel* panel = &applicationLayoutData->panelGroup.panel;
    EditorWindow* currentWindow = applicationLayoutData->currentWindow;
    GapBuffer* matchBuffer = &panel->buffer;

    char* match = gapToString(matchBuffer);

    i32 index = 0;

    // don't render old selections
    currentWindow->selections.currentAmount = 0;

    while(true){
        Selection selection = gapSeekIndexToMatch(&currentWindow->buffer, match, &index, index + 1);
        if(selection.start == selection.end){
            break;
        }
        bufferAppend<Selection>(&currentWindow->selections, selection);
    }

    return false;
}


bool findAction(void* data0){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*)data0;
    Panel* panel = &applicationLayoutData->panelGroup.panel;
    EditorWindow* currentWindow = applicationLayoutData->currentWindow;
    GapBuffer* matchBuffer = &panel->buffer;

    char* match = gapToString(matchBuffer);

    Selection selection = gapSeekCursorToMatch(&currentWindow->buffer, match, panel->lastFind + 1);
    if(selection.start != selection.end){
        currentWindow->buffer.selection.start = selection.start;
        currentWindow->buffer.selection.end = selection.end;
        currentWindow->buffer.cursor = selection.end;
        currentWindow->shiftScrollToMiddle = true;

        panel->lastFind = selection.end;
    }

    return false;
}

bool saveFileAction(void* data0){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*)data0;
    Panel* panel = &applicationLayoutData->panelGroup.panel;
    EditorWindow* currentWindow = applicationLayoutData->currentWindow;
    GapBuffer* filenameBuffer = &panel->buffer;

    char* filename = gapToString(filenameBuffer);

    gapWriteFile(&currentWindow->buffer, filename);
    TRACE("Saved file %s\n", currentWindow->buffer.filename);

    currentWindow->backgroundColor = {0, 0.1, 0, 1};

    return true;
}

bool gotoLineAction(void* data0){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*)data0;
    Panel* panel = &applicationLayoutData->panelGroup.panel;
    EditorWindow* currentWindow = applicationLayoutData->currentWindow;
    GapBuffer* lineBuffer = &panel->buffer;

    char* lineString = gapToString(lineBuffer);
    u32 line = readU32FromBuffer((u8*)lineString);

    gapSeekCursorToLine(&currentWindow->buffer, line);
    TRACE("Got to line: %d\n", line);

    // clean panel gap buffer
    return true;
}



// add applicationlayout data or the file pool or some shit as arguments here
bool quickOpenFileAction(void* data0){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*)data0;
    Panel* panel = &applicationLayoutData->panelGroup.panel;
    EditorWindow* currentWindow = applicationLayoutData->currentWindow;
    GapBuffer* filenameBuffer = &panel->buffer;

    char* fileanme = gapToString(filenameBuffer);
    GapBuffer buffer = gapReadFile(fileanme);

    if(!filenameBuffer->data){
        // clean panel buffer
        return false;
    }
////
     currentWindow->buffer = buffer;
//
////    currentWindow->scrollX = 0;
////    currentWindow->scrollY = 0;
//
    cleanSuggestionBuffer(panel->suggestions);
//
    return true;
    // clean panel gap buffer
}

bool openFileAction(void* data0){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*)data0;
    Panel* panel = &applicationLayoutData->panelGroup.panel;
    EditorWindow* currentWindow = applicationLayoutData->currentWindow;
    GapBuffer* filenameBuffer = &panel->buffer;

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

    currentWindow->buffer = buffer;

    currentWindow->scrollX = 0;
    currentWindow->scrollY = 0;

    cleanSuggestionBuffer(panel->suggestions);

    return true;
    // clean panel gap buffer
}
