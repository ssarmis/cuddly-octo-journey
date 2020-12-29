#pragma once

#include "window.h"

void editorWindowKeyActionRemoveCharacterBeforeCursor(void* data){
    EditorWindow* window = (EditorWindow*) data;
    gapRemoveCharacterAt(&window->buffer, window->buffer.cursor);
    gapDecreaseCursor(&window->buffer);
}

void editorWindowKeyActionRemoveCharacterOnCursor(void* data){
    EditorWindow* window = (EditorWindow*) data;
    gapRemoveCharacterNearAt(&window->buffer, window->buffer.cursor);
}

inline void editorWindowMoveCursorToBegginingOfLine(EditorWindow* window){
    i32 old = window->buffer.cursor;
    gapSeekCursorToPreviousNewline(&window->buffer);
    if(old == window->buffer.cursor){
        // we don't need to do anything, this thing didn't move at all
        return;
    }
    u32 convertedCoordinate = UserToGap(window->buffer.gap, window->buffer.cursor);

    if(isSpacingCharacter(window->buffer.data[convertedCoordinate])){
        gapIncreaseCursor(&window->buffer);
    }
}

inline void editorWindowMoveCursorToEndOfLine(EditorWindow* window){
    gapSeekCursorToNewline(&window->buffer);
}

void editorWindowKeyActionMoveCursorToBegginingOfLine(void* data){
    EditorWindow* window = (EditorWindow*) data;
    
    editorWindowMoveCursorToBegginingOfLine(window);
    
    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorToEndOfLine(void* data){
    EditorWindow* window = (EditorWindow*) data;
    
    editorWindowMoveCursorToEndOfLine(window);
    
    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

inline void editorWindowMoveCursorToAboveLine(EditorWindow* window){
    // u32 convertedCoordinate = UserToGap(buffer->gap, buffer->cursor);
    // if(isSpacingCharacter(buffer->data[convertedCoordinate])){
    //     gapDecreaseCursor(buffer);
    // }

    i32 distanceOnCurrentLineToBegging = gapGetDistanceFromPreviousNewline(&window->buffer);
    
    if(!distanceOnCurrentLineToBegging){
        gapSeekCursorToPreviousNewline(&window->buffer);
    } else {
        gapSeekCursorToPreviousNewline(&window->buffer);
        // gapDecreaseCursor(buffer);

        i32 lengthOfAboveLine = gapGetDistanceFromPreviousNewline(&window->buffer);
        if(lengthOfAboveLine > 0 && lengthOfAboveLine > distanceOnCurrentLineToBegging){
            gapSeekCursor(&window->buffer, -(lengthOfAboveLine - distanceOnCurrentLineToBegging));
        }
    }
}

inline void editorWindowMoveCursorToBelowLine(EditorWindow* window){
    i32 distanceOnCurrentLineToBegging = gapGetDistanceFromPreviousNewline(&window->buffer);

    gapSeekCursorToNewline(&window->buffer);
    // gapIncreaseCursor(buffer);

    if(distanceOnCurrentLineToBegging > 0){
        gapSeekCursor(&window->buffer, distanceOnCurrentLineToBegging);
    }
}

void editorWindowKeyActionMoveCursorToAboveLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    editorWindowMoveCursorToAboveLine(window);
    
    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorToBelowLine(void* data){
    EditorWindow* window = (EditorWindow*) data;
    
    editorWindowMoveCursorToBelowLine(window);

    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorLeft(void* data){
    EditorWindow* window = (EditorWindow*) data;
    gapDecreaseCursor(&window->buffer);

    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorRight(void* data){
    EditorWindow* window = (EditorWindow*) data;
    gapIncreaseCursor(&window->buffer);

    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

inline void editorWindowMoveCursor10LinesUp(EditorWindow* window){
    gapSeekCursorINewlinesIfPossible(&window->buffer, -10);
}

inline void editorWindowMoveCursor10LinesDown(EditorWindow* window){
    gapSeekCursorINewlinesIfPossible(&window->buffer, 10);
}

void editorWindowKeyActionMoveCursor10LinesUp(void* data){
    EditorWindow* window = (EditorWindow*) data;
    
    editorWindowMoveCursor10LinesUp(window);

    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursor10LinesDown(void* data){
    EditorWindow* window = (EditorWindow*) data;

    editorWindowMoveCursor10LinesDown(window);

    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

inline void editorWindowMoveCursorOverWordLeft(EditorWindow* window){
    gapSeekCursorToPreviousCapitalOrSpace(&window->buffer);

    i32 convertedCoordinate = UserToGap(window->buffer.gap, window->buffer.cursor);
    if(isSpacingCharacter(window->buffer.data[convertedCoordinate])){
        gapIncreaseCursor(&window->buffer);
    }
}

inline void editorWindowMoveCursorOverWordRight(EditorWindow* window){
    gapSeekCursorToCapitalOrSpace(&window->buffer);
}

void editorWindowKeyActionMoveCursorOverWordLeft(void* data){
    EditorWindow* window = (EditorWindow*) data;

    editorWindowMoveCursorOverWordLeft(window);
    
    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorOverWordRight(void* data){
    EditorWindow* window = (EditorWindow*) data;


    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorOnePageDown(void* data){
    EditorWindow* window = (EditorWindow*) data;

    u32 heightInLines = window->height / FONT_HEIGHT;
    gapSeekCursorINewlinesIfPossible(&window->buffer, heightInLines);
}

void editorWindowKeyActionMoveCursorOnePageUp(void* data){
    EditorWindow* window = (EditorWindow*) data;

    u32 heightInLines = window->height / FONT_HEIGHT;
    gapSeekCursorINewlinesIfPossible(&window->buffer, -heightInLines);
}

void editorWindowKeyActionMoveCursorAndSelectToAboveLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->buffer.selection.start == window->buffer.cursor){
            editorWindowMoveCursorToAboveLine(window);
            window->buffer.selection.start = window->buffer.cursor;
        } else {
            editorWindowMoveCursorToAboveLine(window);
            window->buffer.selection.end = window->buffer.cursor;
        }
    } else {
        window->buffer.selection.end = window->buffer.cursor;
        editorWindowMoveCursorToAboveLine(window);
        window->buffer.selection.start = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectToBelowLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->buffer.selection.start == window->buffer.cursor){
            editorWindowMoveCursorToBelowLine(window);
            window->buffer.selection.start = window->buffer.cursor;
        } else {
            editorWindowMoveCursorToBelowLine(window);
            window->buffer.selection.end = window->buffer.cursor;
        }
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowMoveCursorToBelowLine(window);
        window->buffer.selection.end = window->buffer.cursor;
    }
}


void editorWindowKeyActionMoveCursorAndSelectLeft(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->buffer.selection.start == window->buffer.cursor){
            // we are at the start of selection
            gapDecreaseCursor(&window->buffer);
            window->buffer.selection.start = window->buffer.cursor;
        } else {
            // we are at the end of selection
            gapDecreaseCursor(&window->buffer);
            window->buffer.selection.end = window->buffer.cursor;
        }
    } else {
        window->buffer.selection.end = window->buffer.cursor;
        gapDecreaseCursor(&window->buffer);
        window->buffer.selection.start = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectRight(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->buffer.selection.start == window->buffer.cursor){
            // we are at the start of selection
            gapIncreaseCursor(&window->buffer);
            window->buffer.selection.start = window->buffer.cursor;
        } else {
            // we are at the end of selection
            gapIncreaseCursor(&window->buffer);
            window->buffer.selection.end = window->buffer.cursor;
        }
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        gapIncreaseCursor(&window->buffer);
        window->buffer.selection.end = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelect10LinesUp(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->buffer.selection.start == window->buffer.cursor){
            editorWindowMoveCursor10LinesUp(window);
            window->buffer.selection.start = window->buffer.cursor;
        } else {
            editorWindowMoveCursor10LinesUp(window);
            window->buffer.selection.end = window->buffer.cursor;
        }
    } else {
        window->buffer.selection.end = window->buffer.cursor;
        editorWindowMoveCursor10LinesUp(window);
        window->buffer.selection.start = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelect10LinesDown(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->buffer.selection.start == window->buffer.cursor){
            editorWindowMoveCursor10LinesDown(window);
            window->buffer.selection.start = window->buffer.cursor;
        } else {
            editorWindowMoveCursor10LinesDown(window);
            window->buffer.selection.end = window->buffer.cursor;
        }
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowMoveCursor10LinesDown(window);
        window->buffer.selection.end = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectOverWordLeft(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->buffer.selection.start == window->buffer.cursor){
            editorWindowMoveCursorOverWordLeft(window);
            window->buffer.selection.start = window->buffer.cursor;
        } else {
            editorWindowMoveCursorOverWordLeft(window);
            window->buffer.selection.end = window->buffer.cursor;
        }
    } else {
        window->buffer.selection.end = window->buffer.cursor;
        editorWindowMoveCursorOverWordLeft(window);
        window->buffer.selection.start = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectOverWordRight(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->buffer.selection.start == window->buffer.cursor){
            editorWindowMoveCursorOverWordRight(window);
            window->buffer.selection.start = window->buffer.cursor;
        } else {
            editorWindowMoveCursorOverWordRight(window);
            window->buffer.selection.end = window->buffer.cursor;
        }
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowMoveCursorOverWordRight(window);
        window->buffer.selection.end = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectToBegginingOfLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->buffer.selection.start == window->buffer.cursor){
            editorWindowMoveCursorToBegginingOfLine(window);
            window->buffer.selection.start = window->buffer.cursor;
        } else {
            editorWindowMoveCursorToBegginingOfLine(window);
            window->buffer.selection.end = window->buffer.cursor;
        }
        
        if(window->buffer.selection.end < window->buffer.selection.start){
            i32 aux = window->buffer.selection.end;
            window->buffer.selection.end = window->buffer.selection.start;
            window->buffer.selection.start = aux;
        }
    } else {
        window->buffer.selection.end = window->buffer.cursor;
        editorWindowMoveCursorToBegginingOfLine(window);
        window->buffer.selection.start = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectToEndOfLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        if(window->buffer.selection.start == window->buffer.cursor){
            editorWindowMoveCursorToEndOfLine(window);
            window->buffer.selection.start = window->buffer.cursor;
        } else {
            editorWindowMoveCursorToEndOfLine(window);
            window->buffer.selection.end = window->buffer.cursor;
        }
        
        if(window->buffer.selection.end < window->buffer.selection.start){
            i32 aux = window->buffer.selection.end;
            window->buffer.selection.end = window->buffer.selection.start;
            window->buffer.selection.start = aux;
        }
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowMoveCursorToEndOfLine(window);
        window->buffer.selection.end = window->buffer.cursor;
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
