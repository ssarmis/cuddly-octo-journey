#pragma once

#include "general.h"

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

void keyActionMoveCursorToAboveLine(void* data){
}

void keyActionMoveCursorToBelowLine(void* data){
}

void keyActionMoveCursorLeft(void* data){
}

void keyActionMoveCursorRight(void* data){
}

void keyActionMoveCursor10LinesUp(void* data){
}

void keyActionMoveCursor10LinesDown(void* data){
}

void keyActionMoveCursorOverWordLeft(void* data){
}

void keyActionMoveCursorOverWordRight(void* data){
}


void keyBindingInitialize(KeyboardBindingManager* keyboardBindingManager){
    keyBindingAddEntry(keyboardBindingManager, KEY_UP,     keyActionMoveCursorToAboveLine);
    keyBindingAddEntry(keyboardBindingManager, KEY_DOWN,   keyActionMoveCursorToBelowLine);
    keyBindingAddEntry(keyboardBindingManager, KEY_LEFT,   keyActionMoveCursorLeft);
    keyBindingAddEntry(keyboardBindingManager, KEY_RIGHT,  keyActionMoveCursorRight);

    keyBindingAddEntry(keyboardBindingManager, KEY_CTRL | KEY_UP,     keyActionMoveCursor10LinesUp);
    keyBindingAddEntry(keyboardBindingManager, KEY_CTRL | KEY_DOWN,   keyActionMoveCursor10LinesDown);
    keyBindingAddEntry(keyboardBindingManager, KEY_CTRL | KEY_LEFT,   keyActionMoveCursorOverWordLeft);
    keyBindingAddEntry(keyboardBindingManager, KEY_CTRL | KEY_RIGHT,  keyActionMoveCursorOverWordRight);
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












