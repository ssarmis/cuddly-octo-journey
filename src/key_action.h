#pragma once

#include "general.h"
#include "editor_window.h"
#include "panel.h"

// 00000000 00000000

#define KEY_CTRL        0  
#define KEY_ALT         0
#define KEY_SHIFT       0

#define KEY_BACKSPACE   0
#define KEY_DELETE      0

#define KEY_PAGEUP      0
#define KEY_PAGEDOWN    0

#define KEY_HOME        0
#define KEY_END         0

#define KEY_LEFT        0
#define KEY_RIGHT       0
#define KEY_UP          0
#define KEY_DOWN        0

typedef void (*Action)(void*);

struct KeyActionData {
    EditorWindow* currentWindow;
    Panel* currentPanel;
};

struct KeyAction {
    u32 key;
    Action action;
};

extern Buffer<KeyAction> keybindings;

void kayActionSeekCursorLeftToSymbolOrSpace(void* data){
}

void kayActionSeekCursorRightToSymbolOrSpace(void* data){
}

void keyActionSeekCursorUp10Lines(void* data){
}

void keyActionSeekCursorDown10Lines(void* data){
}

void keyActionSeekCursorToAboveLine(void* data){
}

void keyActionSeekCursorToBelowLine(void* data){
}

void kayActionSeekCursorLeft(void* data){
}

void kayActionSeekCursorRight(void* data){
}

void keyActionRemoveCharacterBeforeCursor(void* data){
}

void keyActionRemoveCharacterAtCursor(void* data){
}

void keyActionRemoveCharacterBeforeCursorUntilSymbolOrSpace(void* data){
}

void keyActionRemoveCharacterAfterCursorUntilSymbolOrSpace(void* data){
}

void keyActionSeekCursorUpAPage(void* data){
}

void keyActionSeekCursorDownAPage(void* data){
}

void keyActionSeekCursorLeftToBegginingOfLine(void* data){
}

void keyActionSeekCursorRightToEndOfLine(void* data){
}
