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

void editorWindowKeyActionMoveCursorToBegginingOfLine(void* data){
    EditorWindow* window = (EditorWindow*) data;
    gapSeekCursorToPreviousNewline(&window->buffer);
    u32 convertedCoordinate = UserToGap(window->buffer.gap, window->buffer.cursor);
    if(isSpacingCharacter(window->buffer.data[convertedCoordinate])){
        gapIncreaseCursor(&window->buffer);
    }

    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorToEndOfLine(void* data){
    EditorWindow* window = (EditorWindow*) data;
    gapSeekCursorToNewline(&window->buffer);

    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorToAboveLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

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

    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorToBelowLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 distanceOnCurrentLineToBegging = gapGetDistanceFromPreviousNewline(&window->buffer);

    gapSeekCursorToNewline(&window->buffer);
    // gapIncreaseCursor(buffer);

    if(distanceOnCurrentLineToBegging > 0){
        gapSeekCursor(&window->buffer, distanceOnCurrentLineToBegging);
    }

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

void editorWindowKeyActionMoveCursor10LinesUp(void* data){
    EditorWindow* window = (EditorWindow*) data;
    gapSeekCursorINewlinesIfPossible(&window->buffer, -10);

    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursor10LinesDown(void* data){
    EditorWindow* window = (EditorWindow*) data;
    gapSeekCursorINewlinesIfPossible(&window->buffer, 10);

    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorOverWordLeft(void* data){
    EditorWindow* window = (EditorWindow*) data;

    gapSeekCursorToPreviousCapitalOrSpace(&window->buffer);

    i32 convertedCoordinate = UserToGap(window->buffer.gap, window->buffer.cursor);
    if(isSpacingCharacter(window->buffer.data[convertedCoordinate])){
        gapIncreaseCursor(&window->buffer);
    }

    if(gapGetSelectionSize(&window->buffer)){
        window->buffer.selection.end = window->buffer.selection.start;
    }
}

void editorWindowKeyActionMoveCursorOverWordRight(void* data){
    EditorWindow* window = (EditorWindow*) data;

    gapSeekCursorToCapitalOrSpace(&window->buffer);

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
        editorWindowKeyActionMoveCursorToAboveLine(data);
        window->buffer.selection.end = window->buffer.cursor;
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowKeyActionMoveCursorToAboveLine(data);
        window->buffer.selection.end = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectToBelowLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        editorWindowKeyActionMoveCursorToBelowLine(data);
        window->buffer.selection.end = window->buffer.cursor;
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowKeyActionMoveCursorToBelowLine(data);
        window->buffer.selection.end = window->buffer.cursor;
    }
}


void editorWindowKeyActionMoveCursorAndSelectLeft(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        editorWindowKeyActionMoveCursorLeft(data);
        window->buffer.selection.end = window->buffer.cursor;
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowKeyActionMoveCursorLeft(data);
        window->buffer.selection.end = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectRight(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        editorWindowKeyActionMoveCursorRight(data);
        window->buffer.selection.end = window->buffer.cursor;
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowKeyActionMoveCursorRight(data);
        window->buffer.selection.end = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelect10LinesUp(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        editorWindowKeyActionMoveCursor10LinesUp(data);
        window->buffer.selection.end = window->buffer.cursor;
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowKeyActionMoveCursor10LinesUp(data);
        window->buffer.selection.end = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelect10LinesDown(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        editorWindowKeyActionMoveCursor10LinesDown(data);
        window->buffer.selection.end = window->buffer.cursor;
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowKeyActionMoveCursor10LinesDown(data);
        window->buffer.selection.end = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectOverWordLeft(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        editorWindowKeyActionMoveCursorOverWordLeft(data);
        window->buffer.selection.end = window->buffer.cursor;
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowKeyActionMoveCursorOverWordLeft(data);
        window->buffer.selection.end = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectOverWordRight(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        editorWindowKeyActionMoveCursorOverWordRight(data);
        window->buffer.selection.end = window->buffer.cursor;
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowKeyActionMoveCursorOverWordRight(data);
        window->buffer.selection.end = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectToBegginingOfLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        editorWindowKeyActionMoveCursorToBegginingOfLine(data);
        window->buffer.selection.end = window->buffer.cursor;
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowKeyActionMoveCursorToBegginingOfLine(data);
        window->buffer.selection.end = window->buffer.cursor;
    }
}

void editorWindowKeyActionMoveCursorAndSelectToEndOfLine(void* data){
    EditorWindow* window = (EditorWindow*) data;

    i32 selectionSize = gapGetSelectionSize(&window->buffer);

    if(selectionSize){
        // NOTE(Sarmis) we already have a selection going on, so, just move the end
        editorWindowKeyActionMoveCursorToEndOfLine(data);
        window->buffer.selection.end = window->buffer.cursor;
    } else {
        window->buffer.selection.start = window->buffer.cursor;
        editorWindowKeyActionMoveCursorToEndOfLine(data);
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

    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_SHIFT | KEY_UP,       editorWindowKeyActionMoveCursorAndSelect10LinesUp);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_SHIFT | KEY_DOWN,     editorWindowKeyActionMoveCursorAndSelect10LinesDown);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_SHIFT | KEY_LEFT,     editorWindowKeyActionMoveCursorAndSelectOverWordLeft);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_SHIFT | KEY_RIGHT,    editorWindowKeyActionMoveCursorAndSelectOverWordRight);

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

    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_UP,                   editorWindowKeyActionMoveCursor10LinesUp);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_DOWN,                 editorWindowKeyActionMoveCursor10LinesDown);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_LEFT,                 editorWindowKeyActionMoveCursorOverWordLeft);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_RIGHT,                editorWindowKeyActionMoveCursorOverWordRight);
}
