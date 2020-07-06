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

// 0000 0000 0000 1111 0000 0000 0000 0000
#define KEY_RETURN       (1 << 19)
#define KEY_DELETE       (1 << 18)
#define KEY_BACKSPACE    (1 << 17)
#define KEY_ESCAPE       (1 << 16)

struct KeyboardBinding {
    u32 key;
    void (*keyAction1)(void*);
    void (*keyAction2)(void*, void*);
};

struct KeyboardBindingManager {
    Buffer<KeyboardBinding> keyBindings;
};

void keyBindingAddEntry2(KeyboardBindingManager* keyboardBindingManager, u32 key, void (*keyAction)(void*, void*)){
    KeyboardBinding binding = {};

    binding.key = key;
    binding.keyAction1 = NULL;
    binding.keyAction2 = keyAction;

    bufferAppend<KeyboardBinding>(&keyboardBindingManager->keyBindings, &binding);
}

void keyBindingAddEntry1(KeyboardBindingManager* keyboardBindingManager, u32 key, void (*keyAction)(void*)){
    KeyboardBinding binding = {};

    binding.key = key;
    binding.keyAction1 = keyAction;
    binding.keyAction2 = NULL;


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
    u32 convertedCoordinate = UserToGap(buffer->gap, buffer->cursor);
    if(isSpacingCharacter(buffer->data[convertedCoordinate])){
        gapIncreaseCursor(buffer);
    }
}

void keyActionMoveCursorToEndOfLine(void* data){
    GapBuffer* buffer = (GapBuffer*)data;
    gapSeekCursorToNewline(buffer);
}

void keyActionMoveCursorToAboveLine(void* data){
    GapBuffer* buffer = (GapBuffer*)data;

    // u32 convertedCoordinate = UserToGap(buffer->gap, buffer->cursor);
    // if(isSpacingCharacter(buffer->data[convertedCoordinate])){
    //     gapDecreaseCursor(buffer);
    // }

    i32 distanceOnCurrentLineToBegging = gapGetDistanceFromPreviousNewline(buffer);
    
    if(!distanceOnCurrentLineToBegging){
        gapSeekCursorToPreviousNewline(buffer);
    } else {
        gapSeekCursorToPreviousNewline(buffer);
        // gapDecreaseCursor(buffer);

        i32 lengthOfAboveLine = gapGetDistanceFromPreviousNewline(buffer);
        if(lengthOfAboveLine > 0 && lengthOfAboveLine > distanceOnCurrentLineToBegging){
            gapSeekCursor(buffer, -(lengthOfAboveLine - distanceOnCurrentLineToBegging));
        }
    }
}

void keyActionMoveCursorToBelowLine(void* data){
    GapBuffer* buffer = (GapBuffer*)data;

    i32 distanceOnCurrentLineToBegging = gapGetDistanceFromPreviousNewline(buffer);

    gapSeekCursorToNewline(buffer);
    // gapIncreaseCursor(buffer);

    if(distanceOnCurrentLineToBegging > 0){
        gapSeekCursor(buffer, distanceOnCurrentLineToBegging);
    }
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
    GapBuffer* buffer = (GapBuffer*)data;

    gapSeekCursorToPreviousCapitalOrSpace(buffer);

    i32 convertedCoordinate = UserToGap(buffer->gap, buffer->cursor);
    if(isSpacingCharacter(buffer->data[convertedCoordinate])){
        gapIncreaseCursor(buffer);
    }
}

void keyActionMoveCursorOverWordRight(void* data){
    GapBuffer* buffer = (GapBuffer*)data;

    gapSeekCursorToCapitalOrSpace(buffer);
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












