#pragma once

#include "general.h"

// TODO(Sarmis) this shouldn't be here
#include "gap_buffer.h"

// 1111 0000 0000 0000 0000 0000 0000 0000
#define KEY_CTRL         (1 << 31)
#define KEY_ALT          (1 << 30)
#define KEY_SHIFT        (1 << 29)
#define KEY_TAB          (1 << 28)

// 0000 1111 0000 0000 0000 0000 0000 0000
#define KEY_UP           (1 << 27)
#define KEY_DOWN         (1 << 26)
#define KEY_LEFT         (1 << 25)
#define KEY_RIGHT        (1 << 24)

// 0000 0000 1111 0000 0000 0000 0000 0000
#define KEY_PAGEUP       (1 << 23)
#define KEY_PAGEDOWN     (1 << 22)
#define KEY_END          (1 << 21)
#define KEY_HOME         (1 << 20)

// 0000 0000 0000 1110 0000 0000 0000 0000
#define KEY_RETURN       (1 << 19)
#define KEY_DELETE       (1 << 18)
#define KEY_BACKSPACE    (1 << 17)

struct KeyboardBinding {
    u32 key;
    void (*keyAction)(void*);
};

struct KeyboardBindingManager {
    Buffer<KeyboardBinding> keyBindings;
};

void keyBindingAddEntry(KeyboardBindingManager* keyboardBindingManager, u32 key, void (*keyAction)(void*)){
    KeyboardBinding binding = {
        key, 
        keyAction
    };

    bufferAppend<KeyboardBinding>(&keyboardBindingManager->keyBindings, &binding);
}

void keyActionRemoveCharacterBeforeCursor(void* data){
    GapBuffer* buffer = (GapBuffer*)data;
    gapRemoveCharacterAt(buffer, buffer->cursor);
    gapDecreaseCursor(buffer);
}

void keyActionRemoveCharacterOnCursor(void* data){
    GapBuffer* buffer = (GapBuffer*)data;
    gapRemoveCharacterNearAt(buffer, buffer->cursor);
}

void keyActionMoveCursorToBegginingOfLine(void* data){
    GapBuffer* buffer = (GapBuffer*)data;
    gapSeekCursorToPreviousNewline(buffer);
}

void keyActionMoveCursorToEndOfLine(void* data){
    GapBuffer* buffer = (GapBuffer*)data;
    gapSeekCursorToNewline(buffer);
}

void keyActionMoveCursorToAboveLine(void* data){
}

void keyActionMoveCursorToBelowLine(void* data){
}

void keyActionMoveCursorLeft(void* data){
    GapBuffer* buffer = (GapBuffer*)data;
    gapDecreaseCursor(buffer);
}

void keyActionMoveCursorRight(void* data){
    GapBuffer* buffer = (GapBuffer*)data;
    gapIncreaseCursor(buffer);
}

void keyActionMoveCursor10LinesUp(void* data){
    GapBuffer* buffer = (GapBuffer*)data;
    gapSeekCursorINewlinesIfPossible(buffer, -10);
}

void keyActionMoveCursor10LinesDown(void* data){
    GapBuffer* buffer = (GapBuffer*)data;
    gapSeekCursorINewlinesIfPossible(buffer, 10);
}

void keyActionMoveCursorOverWordLeft(void* data){
}

void keyActionMoveCursorOverWordRight(void* data){
}


void keyBindingInitialize(KeyboardBindingManager* keyboardBindingManager){
    // keyBindingAddEntry(keyboardBindingManager, KEY_RETURN,              keyActionInsertNewline);

    keyBindingAddEntry(keyboardBindingManager, KEY_BACKSPACE,           keyActionRemoveCharacterBeforeCursor);
    keyBindingAddEntry(keyboardBindingManager, KEY_DELETE,              keyActionRemoveCharacterOnCursor);

    keyBindingAddEntry(keyboardBindingManager, KEY_HOME,                keyActionMoveCursorToBegginingOfLine);
    keyBindingAddEntry(keyboardBindingManager, KEY_END,                 keyActionMoveCursorToEndOfLine);

    keyBindingAddEntry(keyboardBindingManager, KEY_UP,                  keyActionMoveCursorToAboveLine);
    keyBindingAddEntry(keyboardBindingManager, KEY_DOWN,                keyActionMoveCursorToBelowLine);
    keyBindingAddEntry(keyboardBindingManager, KEY_LEFT,                keyActionMoveCursorLeft);
    keyBindingAddEntry(keyboardBindingManager, KEY_RIGHT,               keyActionMoveCursorRight);

    keyBindingAddEntry(keyboardBindingManager, KEY_CTRL | KEY_UP,       keyActionMoveCursor10LinesUp);
    keyBindingAddEntry(keyboardBindingManager, KEY_CTRL | KEY_DOWN,     keyActionMoveCursor10LinesDown);
    keyBindingAddEntry(keyboardBindingManager, KEY_CTRL | KEY_LEFT,     keyActionMoveCursorOverWordLeft);
    keyBindingAddEntry(keyboardBindingManager, KEY_CTRL | KEY_RIGHT,    keyActionMoveCursorOverWordRight);
}

KeyboardBinding keyBindingGetBindingByKey(KeyboardBindingManager* keyboardBindingManager, u32 key){
    KeyboardBinding result = {};

    for(int i = 0; i < keyboardBindingManager->keyBindings.currentAmount; ++i){
        if(keyboardBindingManager->keyBindings.array[i].key == key){
            return keyboardBindingManager->keyBindings.array[i];
        }
    }

    return result;
}












