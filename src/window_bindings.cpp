#pragma once

#include "window_bindings.h"
#include "window.h"

void editorWindowKeyActionRemoveCharacterBeforeCursor(void* data){
    EditorWindow* window = (EditorWindow*) data;
    
    if(gapGetSelectionSize(&window->currentFile->buffer)){
        String removedString = gapGetSubString(&window->currentFile->buffer, window->currentFile->buffer.selection.start, window->currentFile->buffer.selection.end);
        editorWindowAppendRemoveAction(window, removedString, window->currentFile->buffer.selection.start);

        gapRemoveCharactersInRange(&window->currentFile->buffer, window->currentFile->buffer.selection.start, window->currentFile->buffer.selection.end);
        window->currentFile->buffer.cursor = window->currentFile->buffer.selection.start;
        window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    } else {
        if(window->currentFile->buffer.cursor - 1 >= 0){
            String removedString = gapGetSubString(&window->currentFile->buffer, window->currentFile->buffer.cursor - 1, window->currentFile->buffer.cursor);
            editorWindowAppendRemoveAction(window, removedString, window->currentFile->buffer.cursor - 1);

            gapRemoveCharacterAt(&window->currentFile->buffer, window->currentFile->buffer.cursor);
        }
    }

    gapDecreaseCursor(&window->currentFile->buffer);
}

void editorWindowKeyActionRemoveCharacterOnCursor(void* data){
    EditorWindow* window = (EditorWindow*) data;

    if(gapGetSelectionSize(&window->currentFile->buffer)){
        String removedString = gapGetSubString(&window->currentFile->buffer, window->currentFile->buffer.selection.start, window->currentFile->buffer.selection.end);
        editorWindowAppendRemoveAction(window, removedString, window->currentFile->buffer.selection.start);

        gapRemoveCharactersInRange(&window->currentFile->buffer, window->currentFile->buffer.selection.start, window->currentFile->buffer.selection.end);
        window->currentFile->buffer.cursor = window->currentFile->buffer.selection.start;
        window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    } else {
        if(window->currentFile->buffer.cursor + 1 < gapGetAbstractSize(&window->currentFile->buffer)){
            String removedString = gapGetSubString(&window->currentFile->buffer, window->currentFile->buffer.cursor, window->currentFile->buffer.cursor + 1);
            editorWindowAppendRemoveAction(window, removedString, window->currentFile->buffer.cursor);

            gapRemoveCharacterNearAt(&window->currentFile->buffer, window->currentFile->buffer.cursor);
        }
    }
    
}

inline void editorWindowMoveCursorToBegginingOfLine(EditorWindow* window){
    i32 old = window->currentFile->buffer.cursor;
    gapSeekCursorToPreviousNewline(&window->currentFile->buffer);
    if(old == window->currentFile->buffer.cursor){
        // we don't need to do anything, this thing didn't move at all
        return;
    }

    if(gapGetCursorCharacter(&window->currentFile->buffer) == '\n' || gapGetCursorCharacter(&window->currentFile->buffer) == '\r'){
        gapIncreaseCursor(&window->currentFile->buffer);
    }
}

inline void editorWindowMoveCursorToEndOfLine(EditorWindow* window){
    if(gapGetCursorCharacter(&window->currentFile->buffer) == '\n' || gapGetCursorCharacter(&window->currentFile->buffer) == '\r'){
        return;
    }
    gapSeekCursorToNewline(&window->currentFile->buffer);
}

void editorWindowKeyActionMoveCursorToBegginingOfLine(void* data){
    EditorWindow* window = (EditorWindow*) data;
    
    editorWindowMoveCursorToBegginingOfLine(window);
    
    if(gapGetSelectionSize(&window->currentFile->buffer)){
        window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorToEndOfLine(void* data){
    EditorWindow* window = (EditorWindow*) data;
    
    editorWindowMoveCursorToEndOfLine(window);
    
    if(gapGetSelectionSize(&window->currentFile->buffer)){
        window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    }
}

inline void editorWindowMoveCursorToAboveLine(EditorWindow* window){
    i32 distanceOnCurrentLineToBegging = gapGetDistanceFromPreviousNewline(&window->currentFile->buffer);;
    
    if(!distanceOnCurrentLineToBegging){
        gapSeekCursorToPreviousNewline(&window->currentFile->buffer);
        gapSeekCursorToPreviousNewline(&window->currentFile->buffer);

        gapIncreaseCursor(&window->currentFile->buffer);
    } else {
        gapSeekCursorToPreviousNewline(&window->currentFile->buffer);

        i32 clone = window->currentFile->buffer.cursor;
        gapDecreaseCursor(&window->currentFile->buffer);

        i32 lengthOfAboveLine = gapGetDistanceFromPreviousNewline(&window->currentFile->buffer);
        if(!lengthOfAboveLine){
            window->currentFile->buffer.cursor = clone;
        } else if(lengthOfAboveLine > 0 && lengthOfAboveLine > distanceOnCurrentLineToBegging){
            gapSeekCursor(&window->currentFile->buffer, -(lengthOfAboveLine - distanceOnCurrentLineToBegging));
        }
    }
}

inline void editorWindowMoveCursorToBelowLine(EditorWindow* window){
	    i32 distanceOnCurrentLineToBegging = gapGetDistanceFromPreviousNewline(&window->currentFile->buffer);

    if(!distanceOnCurrentLineToBegging){
        if(isSpacingCharacter(gapGetCursorCharacter(&window->currentFile->buffer))){
            gapIncreaseCursor(&window->currentFile->buffer);
        } else {
            gapSeekCursorToNewline(&window->currentFile->buffer);
            gapIncreaseCursor(&window->currentFile->buffer);
        }
    } else {
        gapSeekCursorToNewline(&window->currentFile->buffer);
        gapSeekCursor(&window->currentFile->buffer, distanceOnCurrentLineToBegging);
    }
}

void editorWindowKeyActionMoveCursorToAboveLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    editorWindowMoveCursorToAboveLine(window);
    
    if(gapGetSelectionSize(&window->currentFile->buffer)){
        window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorToBelowLine(void* data){
    EditorWindow* window = (EditorWindow*) data;
    
    editorWindowMoveCursorToBelowLine(window);

    if(gapGetSelectionSize(&window->currentFile->buffer)){
        window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorLeft(void* data){
    EditorWindow* window = (EditorWindow*) data;
    gapDecreaseCursor(&window->currentFile->buffer);

    if(gapGetSelectionSize(&window->currentFile->buffer)){
	    window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorRight(void* data){
    EditorWindow* window = (EditorWindow*) data;
    gapIncreaseCursor(&window->currentFile->buffer);

    if(gapGetSelectionSize(&window->currentFile->buffer)){
        window->currentFile->buffer.cursor = window->currentFile->buffer.selection.end;
        window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    }
}

inline void editorWindowMoveCursor10LinesUp(EditorWindow* window){
    for(int i = 0; i < 10; ++i){
        editorWindowKeyActionMoveCursorToAboveLine(window);
    }
}

inline void editorWindowMoveCursor10LinesDown(EditorWindow* window){
    for(int i = 0; i < 10; ++i){
        editorWindowKeyActionMoveCursorToBelowLine(window);
    }
}

void editorWindowKeyActionMoveCursor10LinesUp(void* data){
    EditorWindow* window = (EditorWindow*) data;
    
    editorWindowMoveCursor10LinesUp(window);

    if(gapGetSelectionSize(&window->currentFile->buffer)){
        window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursor10LinesDown(void* data){
    EditorWindow* window = (EditorWindow*) data;

    editorWindowMoveCursor10LinesDown(window);

    if(gapGetSelectionSize(&window->currentFile->buffer)){
        window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    }
}

inline void editorWindowMoveCursorOverWordLeft(EditorWindow* window){
    i32 clone = window->currentFile->buffer.cursor;
    gapSeekCursorToPreviousSymbolOrSpace(&window->currentFile->buffer);
    if(clone == window->currentFile->buffer.cursor){
        gapDecreaseCursor(&window->currentFile->buffer);
    }
}

inline void editorWindowMoveCursorOverWordRight(EditorWindow* window){
    gapSeekCursorToSymbolOrSpace(&window->currentFile->buffer);
}

void editorWindowKeyActionMoveCursorOverWordLeft(void* data){
    EditorWindow* window = (EditorWindow*) data;

    editorWindowMoveCursorOverWordLeft(window);
    
    if(gapGetSelectionSize(&window->currentFile->buffer)){
        window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorOverWordRight(void* data){
    EditorWindow* window = (EditorWindow*) data;
    
    editorWindowMoveCursorOverWordRight(window);

    if(gapGetSelectionSize(&window->currentFile->buffer)){
        window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    }
}


void editorWindowKeyActionRemoveWordLeft(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 end = window->currentFile->buffer.cursor;
    editorWindowMoveCursorOverWordLeft(window);
    i32 start = window->currentFile->buffer.cursor;

    if(start == end){
        return;
    }

    if(start < 0 || end > gapGetAbstractSize(&window->currentFile->buffer)){
        return;
    }        

    String removedString = gapGetSubString(&window->currentFile->buffer, start, end);
    editorWindowAppendRemoveAction(window, removedString, start);

    gapRemoveCharactersInRange(&window->currentFile->buffer, start, end);
    window->currentFile->buffer.cursor = start;

    if(gapGetSelectionSize(&window->currentFile->buffer)){
        window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    }
}

void editorWindowKeyActionRemoveWordRight(void* data){
    EditorWindow* window = (EditorWindow*) data;

    u32 start = window->currentFile->buffer.cursor;
    editorWindowMoveCursorOverWordRight(window);
    u32 end = window->currentFile->buffer.cursor;

    if(start == end){
        return;
    }

    if(start < 0 || end > gapGetAbstractSize(&window->currentFile->buffer)){
        return;
    }        

    String removedString = gapGetSubString(&window->currentFile->buffer, start, end);
    editorWindowAppendRemoveAction(window, removedString, start);

    gapRemoveCharactersInRange(&window->currentFile->buffer, start, end);
    window->currentFile->buffer.cursor = start;

    if(gapGetSelectionSize(&window->currentFile->buffer)){
        window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
    }
}


void editorWindowKeyActionMoveCursorOnePageDown(void* data){
    EditorWindow* window = (EditorWindow*) data;

    u32 heightInLines = window->height / FONT_HEIGHT;
    // gapSeekCursorINewlinesIfPossible(&window->currentFile->buffer, heightInLines);
    for(int i = 0; i < heightInLines; ++i){
        editorWindowKeyActionMoveCursorToBelowLine(window);
    }
}

void editorWindowKeyActionMoveCursorOnePageUp(void* data){
    EditorWindow* window = (EditorWindow*) data;

    u32 heightInLines = window->height / FONT_HEIGHT;
    // gapSeekCursorINewlinesIfPossible(&window->currentFile->buffer, -heightInLines);
    for(int i = 0; i < heightInLines; ++i){
        editorWindowKeyActionMoveCursorToAboveLine(window);
    }
}

void editorWindowKeyActionMoveCursorAndSelectToAboveLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->currentFile->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->currentFile->buffer.selection.start == window->currentFile->buffer.cursor){
            editorWindowMoveCursorToAboveLine(window);
            window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        } else {
            editorWindowMoveCursorToAboveLine(window);
            window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        }
    } else {
        window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        editorWindowMoveCursorToAboveLine(window);
        window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectToBelowLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->currentFile->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->currentFile->buffer.selection.start == window->currentFile->buffer.cursor){
            editorWindowMoveCursorToBelowLine(window);
            window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        } else {
            editorWindowMoveCursorToBelowLine(window);
            window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        }
    } else {
        window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        editorWindowMoveCursorToBelowLine(window);
        window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
    }
}


void editorWindowKeyActionMoveCursorAndSelectLeft(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->currentFile->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->currentFile->buffer.selection.start == window->currentFile->buffer.cursor){
            // we are at the start of selection
            gapDecreaseCursor(&window->currentFile->buffer);
            window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        } else {
            // we are at the end of selection
            gapDecreaseCursor(&window->currentFile->buffer);
            window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        }
    } else {
        window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        gapDecreaseCursor(&window->currentFile->buffer);
        window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectRight(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->currentFile->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->currentFile->buffer.selection.start == window->currentFile->buffer.cursor){
            // we are at the start of selection
            gapIncreaseCursor(&window->currentFile->buffer);
            window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        } else {
            // we are at the end of selection
            gapIncreaseCursor(&window->currentFile->buffer);
            window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        }
    } else {
        window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        gapIncreaseCursor(&window->currentFile->buffer);
        window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelect10LinesUp(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->currentFile->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->currentFile->buffer.selection.start == window->currentFile->buffer.cursor){
            editorWindowMoveCursor10LinesUp(window);
            window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        } else {
            editorWindowMoveCursor10LinesUp(window);
            window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        }
    } else {
        window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        editorWindowMoveCursor10LinesUp(window);
        window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelect10LinesDown(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->currentFile->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->currentFile->buffer.selection.start == window->currentFile->buffer.cursor){
            editorWindowMoveCursor10LinesDown(window);
            window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        } else {
            editorWindowMoveCursor10LinesDown(window);
            window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        }
    } else {
        window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        editorWindowMoveCursor10LinesDown(window);
        window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectOverWordLeft(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->currentFile->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->currentFile->buffer.selection.start == window->currentFile->buffer.cursor){
            editorWindowMoveCursorOverWordLeft(window);
            window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        } else {
            editorWindowMoveCursorOverWordLeft(window);
            window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        }
    } else {
        window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        editorWindowMoveCursorOverWordLeft(window);
        window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectOverWordRight(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->currentFile->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->currentFile->buffer.selection.start == window->currentFile->buffer.cursor){
            editorWindowMoveCursorOverWordRight(window);
            window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        } else {
            editorWindowMoveCursorOverWordRight(window);
            window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        }
    } else {
        window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        editorWindowMoveCursorOverWordRight(window);
        window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectToBegginingOfLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->currentFile->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->currentFile->buffer.selection.start == window->currentFile->buffer.cursor){
            editorWindowMoveCursorToBegginingOfLine(window);
            window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        } else {
            editorWindowMoveCursorToBegginingOfLine(window);
            window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        }
        
        if(window->currentFile->buffer.selection.end < window->currentFile->buffer.selection.start){
            i32 aux = window->currentFile->buffer.selection.end;
            window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
            window->currentFile->buffer.selection.start = aux;
        }
    } else {
        window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        editorWindowMoveCursorToBegginingOfLine(window);
        window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectOnePageDown(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->currentFile->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->currentFile->buffer.selection.start == window->currentFile->buffer.cursor){
            editorWindowKeyActionMoveCursorOnePageDown(window);
            window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        } else {
            editorWindowKeyActionMoveCursorOnePageDown(window);
            window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        }
        
        if(window->currentFile->buffer.selection.end < window->currentFile->buffer.selection.start){
            i32 aux = window->currentFile->buffer.selection.end;
            window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
            window->currentFile->buffer.selection.start = aux;
        }
    } else {
        window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        editorWindowKeyActionMoveCursorOnePageDown(window);
        window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectOnePageUp(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->currentFile->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->currentFile->buffer.selection.start == window->currentFile->buffer.cursor){
            editorWindowKeyActionMoveCursorOnePageUp(window);
            window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        } else {
            editorWindowKeyActionMoveCursorOnePageUp(window);
            window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        }
        
        if(window->currentFile->buffer.selection.end < window->currentFile->buffer.selection.start){
            i32 aux = window->currentFile->buffer.selection.end;
            window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
            window->currentFile->buffer.selection.start = aux;
        }
    } else {
        window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        editorWindowKeyActionMoveCursorOnePageUp(window);
        window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectToEndOfLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->currentFile->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->currentFile->buffer.selection.start == window->currentFile->buffer.cursor){
            editorWindowMoveCursorToEndOfLine(window);
            window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        } else {
            editorWindowMoveCursorToEndOfLine(window);
            window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
        }
        
        if(window->currentFile->buffer.selection.end < window->currentFile->buffer.selection.start){
            i32 aux = window->currentFile->buffer.selection.end;
            window->currentFile->buffer.selection.end = window->currentFile->buffer.selection.start;
            window->currentFile->buffer.selection.start = aux;
        }
    } else {
        window->currentFile->buffer.selection.start = window->currentFile->buffer.cursor;
        editorWindowMoveCursorToEndOfLine(window);
        window->currentFile->buffer.selection.end = window->currentFile->buffer.cursor;
    }
}

void editorWindowKeyBindingInitialize(KeyboardBindingManager* keyboardBindingManager){
    // NOTE(Sarmis) most of these are from the keybaord_bindings.h but they are an
    // editorWindow version, that takes the editorWindow as a parameter, no just a gapbuffer

    keyBindingAddEntry1(keyboardBindingManager, KEY_SHIFT | KEY_UP,                  editorWindowKeyActionMoveCursorAndSelectToAboveLine);
    keyBindingAddEntry1(keyboardBindingManager, KEY_SHIFT | KEY_DOWN,                editorWindowKeyActionMoveCursorAndSelectToBelowLine);
    keyBindingAddEntry1(keyboardBindingManager, KEY_SHIFT | KEY_LEFT,                editorWindowKeyActionMoveCursorAndSelectLeft);
    keyBindingAddEntry1(keyboardBindingManager, KEY_SHIFT | KEY_RIGHT,               editorWindowKeyActionMoveCursorAndSelectRight);

    keyBindingAddEntry1(keyboardBindingManager, KEY_SHIFT | KEY_HOME,                editorWindowKeyActionMoveCursorAndSelectToBegginingOfLine);
    keyBindingAddEntry1(keyboardBindingManager, KEY_SHIFT | KEY_END,                 editorWindowKeyActionMoveCursorAndSelectToEndOfLine);

    keyBindingAddEntry1(keyboardBindingManager, KEY_SHIFT | KEY_PAGEDOWN,            editorWindowKeyActionMoveCursorAndSelectOnePageDown);
    keyBindingAddEntry1(keyboardBindingManager, KEY_SHIFT | KEY_PAGEUP,              editorWindowKeyActionMoveCursorAndSelectOnePageUp);
#ifdef __unix__
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_SHIFT | KEY_UP,       editorWindowKeyActionMoveCursorAndSelect10LinesUp);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_SHIFT | KEY_DOWN,     editorWindowKeyActionMoveCursorAndSelect10LinesDown);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_SHIFT | KEY_LEFT,     editorWindowKeyActionMoveCursorAndSelectOverWordLeft);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_SHIFT | KEY_RIGHT,    editorWindowKeyActionMoveCursorAndSelectOverWordRight);
#elif defined __APPLE__
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | KEY_SHIFT | KEY_UP,       editorWindowKeyActionMoveCursorAndSelect10LinesUp);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | KEY_SHIFT | KEY_DOWN,     editorWindowKeyActionMoveCursorAndSelect10LinesDown);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | KEY_SHIFT | KEY_LEFT,     editorWindowKeyActionMoveCursorAndSelectOverWordLeft);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | KEY_SHIFT | KEY_RIGHT,    editorWindowKeyActionMoveCursorAndSelectOverWordRight);
#endif
    
    keyBindingAddEntry1(keyboardBindingManager, KEY_BACKSPACE,                       editorWindowKeyActionRemoveCharacterBeforeCursor);
    keyBindingAddEntry1(keyboardBindingManager, KEY_DELETE,                          editorWindowKeyActionRemoveCharacterOnCursor);

    keyBindingAddEntry1(keyboardBindingManager, KEY_HOME,                            editorWindowKeyActionMoveCursorToBegginingOfLine);
    keyBindingAddEntry1(keyboardBindingManager, KEY_END,                             editorWindowKeyActionMoveCursorToEndOfLine);

    keyBindingAddEntry1(keyboardBindingManager, KEY_PAGEDOWN,                        editorWindowKeyActionMoveCursorOnePageDown);
    keyBindingAddEntry1(keyboardBindingManager, KEY_PAGEUP,                          editorWindowKeyActionMoveCursorOnePageUp);

    keyBindingAddEntry1(keyboardBindingManager, KEY_UP,                              editorWindowKeyActionMoveCursorToAboveLine);
    keyBindingAddEntry1(keyboardBindingManager, KEY_DOWN,                            editorWindowKeyActionMoveCursorToBelowLine);
    keyBindingAddEntry1(keyboardBindingManager, KEY_LEFT,                            editorWindowKeyActionMoveCursorLeft);
    keyBindingAddEntry1(keyboardBindingManager, KEY_RIGHT,                           editorWindowKeyActionMoveCursorRight);

#ifdef __unix__
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_BACKSPACE,            editorWindowKeyActionRemoveWordLeft);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_DELETE,               editorWindowKeyActionRemoveWordRight);

    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_UP,                   editorWindowKeyActionMoveCursor10LinesUp);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_DOWN,                 editorWindowKeyActionMoveCursor10LinesDown);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_LEFT,                 editorWindowKeyActionMoveCursorOverWordLeft);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_RIGHT,                editorWindowKeyActionMoveCursorOverWordRight);
#elif defined __APPLE__
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | KEY_UP,                   editorWindowKeyActionMoveCursor10LinesUp);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | KEY_DOWN,                 editorWindowKeyActionMoveCursor10LinesDown);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | KEY_LEFT,                 editorWindowKeyActionMoveCursorOverWordLeft);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | KEY_RIGHT,                editorWindowKeyActionMoveCursorOverWordRight);
#endif
}
  