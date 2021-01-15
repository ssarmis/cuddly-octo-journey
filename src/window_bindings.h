#pragma once

#include "keyboard_bindings.h"

struct EditorWindow;

void editorWindowKeyActionRemoveCharacterBeforeCursor(void* data);
void editorWindowKeyActionRemoveCharacterOnCursor(void* data);

inline void editorWindowMoveCursorToBegginingOfLine(EditorWindow* window);
inline void editorWindowMoveCursorToEndOfLine(EditorWindow* window);

void editorWindowKeyActionMoveCursorToBegginingOfLine(void* data);
void editorWindowKeyActionMoveCursorToEndOfLine(void* data);

inline void editorWindowMoveCursorToAboveLine(EditorWindow* window);
inline void editorWindowMoveCursorToBelowLine(EditorWindow* window);

void editorWindowKeyActionMoveCursorToAboveLine(void* data);
void editorWindowKeyActionMoveCursorToBelowLine(void* data);

void editorWindowKeyActionMoveCursorLeft(void* data);
void editorWindowKeyActionMoveCursorRight(void* data);

inline void editorWindowMoveCursor10LinesUp(EditorWindow* window);
inline void editorWindowMoveCursor10LinesDown(EditorWindow* window);

void editorWindowKeyActionMoveCursor10LinesUp(void* data);
void editorWindowKeyActionMoveCursor10LinesDown(void* data);

inline void editorWindowMoveCursorOverWordLeft(EditorWindow* window);
inline void editorWindowMoveCursorOverWordRight(EditorWindow* window);

void editorWindowKeyActionMoveCursorOverWordLeft(void* data);
void editorWindowKeyActionMoveCursorOverWordRight(void* data);

void editorWindowKeyActionRemoveWordLeft(void* data);
void editorWindowKeyActionRemoveWordRight(void* data);

void editorWindowKeyActionMoveCursorOnePageDown(void* data);
void editorWindowKeyActionMoveCursorOnePageUp(void* data);

void editorWindowKeyActionMoveCursorAndSelectToAboveLine(void* data);
void editorWindowKeyActionMoveCursorAndSelectToBelowLine(void* data);

void editorWindowKeyActionMoveCursorAndSelectLeft(void* data);
void editorWindowKeyActionMoveCursorAndSelectRight(void* data);

void editorWindowKeyActionMoveCursorAndSelect10LinesUp(void* data);
void editorWindowKeyActionMoveCursorAndSelect10LinesDown(void* data);

void editorWindowKeyActionMoveCursorAndSelectOverWordLeft(void* data);

void editorWindowKeyActionMoveCursorAndSelectOverWordRight(void* data);
void editorWindowKeyActionMoveCursorAndSelectToBegginingOfLine(void* data);

void editorWindowKeyActionMoveCursorAndSelectOnePageDown(void* data);
void editorWindowKeyActionMoveCursorAndSelectOnePageUp(void* data);

void editorWindowKeyActionMoveCursorAndSelectToEndOfLine(void* data);

void editorWindowKeyBindingInitialize(KeyboardBindingManager* keyboardBindingManager);
  