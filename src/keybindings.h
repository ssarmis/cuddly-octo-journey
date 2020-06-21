#pragma once

#include "general.h"
#include "key_action.h"

#define ADD_KEYBINDING(combination, action) keybindings[combination] = (action)

void kayBindingsInitialization(){
    bufferAppend<KeyAction>(&keybindings, {KEY_HOME, keyActionSeekCursorLeftToBegginingOfLine});
    bufferAppend<KeyAction>(&keybindings, {KEY_END, keyActionSeekCursorRightToEndOfLine});

    bufferAppend<KeyAction>(&keybindings, {KEY_BACKSPACE, keyActionRemoveCharacterBeforeCursor});
    bufferAppend<KeyAction>(&keybindings, {KEY_DELETE, keyActionRemoveCharacterAtCursor});

    bufferAppend<KeyAction>(&keybindings, {KEY_UP, keyActionSeekCursorToAboveLine});
    bufferAppend<KeyAction>(&keybindings, {KEY_DOWN, keyActionSeekCursorToBelowLine});
    bufferAppend<KeyAction>(&keybindings, {KEY_LEFT, kayActionSeekCursorLeft});
    bufferAppend<KeyAction>(&keybindings, {KEY_RIGHT, kayActionSeekCursorRight});

    bufferAppend<KeyAction>(&keybindings, {KEY_CTRL | KEY_UP, keyActionSeekCursorUp10Lines});
    bufferAppend<KeyAction>(&keybindings, {KEY_CTRL | KEY_DOWN, keyActionSeekCursorDown10Lines});
    bufferAppend<KeyAction>(&keybindings, {KEY_CTRL | KEY_LEFT, kayActionSeekCursorLeftToSymbolOrSpace});
    bufferAppend<KeyAction>(&keybindings, {KEY_CTRL | KEY_RIGHT, kayActionSeekCursorRightToSymbolOrSpace});

    bufferAppend<KeyAction>(&keybindings, {KEY_CTRL | KEY_BACKSPACE, keyActionRemoveCharacterBeforeCursorUntilSymbolOrSpace});
    bufferAppend<KeyAction>(&keybindings, {KEY_CTRL | KEY_DELETE, keyActionRemoveCharacterAfterCursorUntilSymbolOrSpace});

    bufferAppend<KeyAction>(&keybindings, {KEY_PAGEUP, keyActionSeekCursorUpAPage});
    bufferAppend<KeyAction>(&keybindings, {KEY_PAGEDOWN, keyActionSeekCursorDownAPage});
}
