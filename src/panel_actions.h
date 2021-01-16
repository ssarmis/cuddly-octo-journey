#pragma once 

#include "window.h"
#include "panel.h"
#include "app.h"
#include "gap_buffer.h"
#include "editor_file.h"

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
        Selection selection = gapSeekIndexToMatch(&currentWindow->currentFile->buffer, match, &index, index + 1);
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

    Selection selection = gapSeekCursorToMatch(&currentWindow->currentFile->buffer, match, panel->lastFind + 1);
    if(selection.start != selection.end){
        currentWindow->currentFile->buffer.selection.start = selection.start;
        currentWindow->currentFile->buffer.selection.end = selection.end;
        currentWindow->currentFile->buffer.cursor = selection.end;
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

    gapWriteFile(&currentWindow->currentFile->buffer, filename);
    TRACE("Saved file %s\n", currentwindow->currentFile->filename.data);

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

    gapSeekCursorToLine(&currentWindow->currentFile->buffer, line);
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
     currentWindow->currentFile->buffer = buffer;
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
    String filenameString = cloneString(filename);

    // if(window->currentFile->filename.data){
    //     // TODO(Sarmis) add some save thing
    //     // even something like a logo or idk
    //     // to indicate that someting was saved
    //     // and make it global so it can be called from anywhere
    //     gapWriteFile(&window->currentFile->buffer);
    //     TRACE("Saved file %s\n", window->currentFile->filename.data);
    // }
    EditorFile* potentialFile = getEditorFileByFullPath(applicationLayoutData->filePool.files, filenameString);
    if(potentialFile){
        currentWindow->currentFile = potentialFile;
        delete[] filenameString.data;
    } else {
        GapBuffer buffer = gapReadFile(filename);

        if(!buffer.data){
            // clean panel buffer
            return false;
        }

        EditorFile newFile = {};
        newFile.buffer = buffer;
        newFile.fullPath = filenameString;

        u32 lastSlash = characterLastOccurence(filenameString, '/');
        if(lastSlash){
            ++lastSlash;
            String substring = subString(filenameString, lastSlash + 1, filenameString.size);
            newFile.filename = substring;
        } else {
            newFile.filename = filenameString;
        }

        bufferAppend<EditorFile>(&applicationLayoutData->filePool.files, newFile);

        currentWindow->currentFile = &applicationLayoutData->filePool.files[applicationLayoutData->filePool.files.currentAmount - 1];
    }

    currentWindow->scrollX = 0;
    currentWindow->scrollY = 0;

    cleanSuggestionBuffer(panel->suggestions);

    return true;
    // clean panel gap buffer
}
