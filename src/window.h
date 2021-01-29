#pragma once

#include "general.h"
#include "shader.h"
#include "font.h"
#include "gap_buffer.h"
#include "math.h"
#include "editor_file.h"

#include "keyboard_bindings.h"
#include "window_bindings.h"

#define CURSOR_BORDER (2)

struct EditorWindowStatusBar {
    v3 position;
    v2 size;
};

enum ActionType {
    ACTION_INSERT,
    ACTION_REMOVE
};

struct Action {
    ActionType type;
    union {
        struct {
            String data;
            u32 location;
        };
        struct {
            u32 start;
            u32 end;
        };
    };
};

struct EditorWindow {
    i32 width;
    i32 height;

    r32 scrollY;
    r32 scrollX;
    r32 scrollTop;
    r32 scrollBottom;
    r32 scrollLeft;
    r32 scrollRight;

    u32 left;
    u32 top;
    u32 bottom;

    m4 transform;
    m4 view;

    v3 cursor;

    v4 backgroundColor;

    Buffer<Action> actions;
    // TODO(Sarmis) preallocate selections
    Buffer<Selection> selections;
    EditorWindowStatusBar statusBar;

    EditorFile* currentFile;
    GapBuffer buffer_; // used for empty buffer, might just treat them as untitled files

    bool currentlyTyping;
    i32 localTime;
    bool visible;
    bool shiftScrollToMiddle;

    Selection temporarySelection;
    v4 temporaryColor;
};

#define STATUS_BAR_HEIGHT (FONT_HEIGHT + FONT_HEIGHT / 8)

static EditorWindow windowCreate(i32 width, i32 height, u32 left, u32 top){
    EditorWindow result = {};

    result.width = width;
    result.height = height;

    result.cursor = v3(0);

    result.left = left;
    result.top = top;
    result.bottom = result.top + result.height;

    result.transform = m4();
    result.view = m4();

    result.scrollX = 0;
    result.scrollY = 0;

    result.scrollTop = result.top;
    result.scrollBottom = result.top + result.height;
    result.scrollLeft = result.left;
    result.scrollRight = result.left + result.width;

    result.currentFile = NULL;
    result.buffer_ = gapCreateEmpty();
    result.backgroundColor = DEFAULT_COLOR_BACKGROUND;

    result.statusBar.size = v2(width, STATUS_BAR_HEIGHT);
    result.statusBar.position = v3(left, height - STATUS_BAR_HEIGHT, 0);

    result.actions = {};
    result.selections = {};

    result.currentlyTyping = false;
    result.localTime = 0;
    result.visible = true;

    result.temporarySelection = {};
    result.temporaryColor = {};

    return result;
}

static KeyboardBindingManager windowKeyboardBindingManager;

#include "keyboard_manager.h"

static void editorWindowAppendInsertAction(EditorWindow* window, u32 start, u32 end){
    Action action = {};
    action.type = ACTION_INSERT;
    action.start = start;
    action.end = end;
    bufferAppend<Action>(&window->actions, action);
}

static void editorWindowAppendRemoveAction(EditorWindow* window, String string, u32 location){
    Action action = {};
    action.type = ACTION_REMOVE;
    action.data = string;
    action.location = location;
    bufferAppend<Action>(&window->actions, action);
}

static void editorWindowTick(EditorWindow* window, KeyboardManager* keyboardManager){
    window->localTime %= 60;
    if(!window->localTime){
        window->currentlyTyping = false;
    }

    if(keyboardManager->currentActiveKeyStroke){
        window->currentlyTyping = true;

        if(!(keyboardManager->currentActiveKeyStroke & KEY_CTRL || keyboardManager->currentActiveKeyStroke & KEY_CMD)){
            char potentialCharacter = keyboardManager->currentActiveKeyStroke & 0xff;
            bool insertedCharacter = false;
            if(isAlphanumericCharacter(potentialCharacter)){

                if(gapGetSelectionSize(&window->currentFile->buffer)){ 
	                editorWindowKeyActionRemoveCharacterOnCursor((void*)window);
                }
                editorWindowAppendInsertAction(window, window->currentFile->buffer.cursor, window->currentFile->buffer.cursor + 1);

                gapInsertCharacterAt(&window->currentFile->buffer, potentialCharacter, window->currentFile->buffer.cursor);
                gapIncreaseCursor(&window->currentFile->buffer);
            } else if(isSpacingCharacter(potentialCharacter)){

                if(gapGetSelectionSize(&window->currentFile->buffer)){ 
	                editorWindowKeyActionRemoveCharacterOnCursor((void*)window);
                }
                editorWindowAppendInsertAction(window, window->currentFile->buffer.cursor, window->currentFile->buffer.cursor + 1);

                gapInsertCharacterAt(&window->currentFile->buffer, potentialCharacter, window->currentFile->buffer.cursor);
                gapIncreaseCursor(&window->currentFile->buffer);
            } else if(keyboardManager->currentActiveKeyStroke == KEY_TAB){ // ONLY TAB, no other key

                if(gapGetSelectionSize(&window->currentFile->buffer)){ 
	                editorWindowKeyActionRemoveCharacterOnCursor((void*)window);
                }
                editorWindowAppendInsertAction(window, window->currentFile->buffer.cursor, window->currentFile->buffer.cursor + 1);
				// NOTE(Sarmis) lets assume \t is 4 spaces
                // gapInsertCharacterAt(&window->currentFile->buffer, '\t', window->currentFile->buffer.cursor);
                // gapIncreaseCursor(&window->currentFile->buffer);
                gapInsertCharacterAt(&window->currentFile->buffer, ' ', window->currentFile->buffer.cursor);
                gapIncreaseCursor(&window->currentFile->buffer);
                gapInsertCharacterAt(&window->currentFile->buffer, ' ', window->currentFile->buffer.cursor);
                gapIncreaseCursor(&window->currentFile->buffer);
                gapInsertCharacterAt(&window->currentFile->buffer, ' ', window->currentFile->buffer.cursor);
                gapIncreaseCursor(&window->currentFile->buffer);
                gapInsertCharacterAt(&window->currentFile->buffer, ' ', window->currentFile->buffer.cursor);
                gapIncreaseCursor(&window->currentFile->buffer);
            } else if(keyboardManager->currentActiveKeyStroke == KEY_RETURN){ // ONLY RETURN, no other key

                if(gapGetSelectionSize(&window->currentFile->buffer)){ 
	                editorWindowKeyActionRemoveCharacterOnCursor((void*)window);
                }
                editorWindowAppendInsertAction(window, window->currentFile->buffer.cursor, window->currentFile->buffer.cursor + 1);

                gapInsertCharacterAt(&window->currentFile->buffer, '\n', window->currentFile->buffer.cursor);
                gapIncreaseCursor(&window->currentFile->buffer);
            }		
        }
    } else {
        char c = gapGetCursorCharacter(&window->currentFile->buffer);
        if(!isSymbolCharacter(c) && !isSpacingCharacter(c)){
            // reuse buffer, don't reallocate
            window->selections.currentAmount = 0;

            u32 clone = window->currentFile->buffer.cursor;
            gapSeekCursorToPreviousSymbolOrSpace(&window->currentFile->buffer);
            u32 start = window->currentFile->buffer.cursor;
            window->currentFile->buffer.cursor = clone;
            gapSeekCursorToSymbolOrSpace(&window->currentFile->buffer);
            u32 end = window->currentFile->buffer.cursor;

            window->currentFile->buffer.cursor = clone;

            if(start != end){
                String word = gapGetSubString(&window->currentFile->buffer, start, end);

                i32 status = icharacterFirstOccurence(word, ' ');
                status += icharacterFirstOccurence(word, '\t');
                // it works, ok?
                if(status == -2){
                    i32 index = 0;
                    while(true){
                        Selection selection = gapSeekIndexToMatch(&window->currentFile->buffer, (char*)word.data, &index, index + 1);
                        if(selection.start == selection.end){
                            break;
                        }
                        bufferAppend<Selection>(&window->selections, selection);
                    }
                }
                delete[] word.data;
            }
        }
    }

    if(window->currentlyTyping){
        window->selections.currentAmount = 0;
        ++window->localTime;
    }

    KeyboardBinding binding = keyBindingGetBindingByKey(&windowKeyboardBindingManager, keyboardManager->currentActiveKeyStroke);
    if(!binding.key){
        return;
    }

    binding.keyAction1(window);
}

static void editorWindowDecideCursorPositionByGapBuffer(EditorWindow* window, FontGL* font){
    for(int i = 0; i < window->currentFile->buffer.cursor; ++i){
        char character = gapCharacterAtIndex(&window->currentFile->buffer, i); 
        switch(character){
            case 0:{
                    // ASSERT(false);
                }
                break;

            case '\t': {
                    window->cursor.x += FONT_HEIGHT * 2;
                }
                break;

            case '\n': {
                    window->cursor.y += FONT_HEIGHT;
                    window->cursor.x = window->left;
                }
                break;

            default: {
                    Glyph glyph = font->glyphs[character - ' '];
                    window->cursor.x += glyph.xadvance;
                }
                break;
        }
    }
}

static void editorWindowScrollIfNeeded(EditorWindow* window){
    if(window->cursor.x >= window->scrollRight){
        i32 distance = window->cursor.x - window->scrollRight + FONT_HEIGHT / 2;
        window->scrollRight += distance;
        window->scrollLeft += distance;
        window->scrollX += distance;

        window->statusBar.position.x += distance;
    } else if(window->cursor.x <= window->scrollLeft){
        i32 distance = window->scrollLeft - window->cursor.x;
        window->scrollRight -= distance;
        window->scrollLeft -= distance;
        window->scrollX -= distance;

        window->statusBar.position.x -= distance;
    }

    if(window->cursor.y <= window->scrollTop){
        i32 distance = window->scrollTop - window->cursor.y;
        window->scrollBottom -= distance;
        window->scrollTop -= distance;
        window->scrollY += distance;

        window->statusBar.position.y -= distance;
    } else if(window->cursor.y >= window->scrollBottom - STATUS_BAR_HEIGHT){
        i32 distance = window->cursor.y + STATUS_BAR_HEIGHT - window->scrollBottom + FONT_HEIGHT;
        window->scrollBottom += distance;
        window->scrollTop += distance;
        window->scrollY -= distance;

        window->statusBar.position.y += distance;
    }

    if(window->shiftScrollToMiddle){
        window->shiftScrollToMiddle = false;
        r32 distanceVertical = window->cursor.y - (window->scrollTop + window->top + window->height / 2);
        r32 distanceHorizontal = window->cursor.x - (window->scrollLeft + window->left + window->width / 2);

        if(distanceVertical > 0){
            window->scrollBottom += distanceVertical;
            window->scrollTop += distanceVertical;
            window->scrollY -= distanceVertical;

            window->statusBar.position.y += distanceVertical;
        }

        if(distanceHorizontal < 0){
            window->scrollRight = window->left + window->width;
            window->scrollLeft = window->left;
            window->scrollX = 0;
            printf("Setting R L X %f %f %f\n", window->scrollRight, window->scrollLeft, window->scrollX);

            window->statusBar.position.x = window->left;
        }
        //  else {
        //     window->scrollRight += distanceHorizontal;
        //     window->scrollLeft += distanceHorizontal;
        //     window->scrollX += distanceHorizontal;

        //     window->statusBar.position.x += distanceHorizontal;
        // }
    }

    window->transform = translate({-window->scrollX, window->scrollY, 0});
}

static void editorWindowRender(EditorWindow* window, 
                        Shader* shader, Shader* shaderUI,
                        RenderBuffer* renderBuffer, RenderBuffer* renderBufferUI, RenderBuffer* renderBufferBackground,
                        FontGL* font,
                        int time, bool activeWindow){
    v2 uvs[4] = {};

    window->cursor = v3(window->left, window->top, 0);
    editorWindowDecideCursorPositionByGapBuffer(window, font);
    
    window->cursor.y += 3; // NOTE(Sarmis) idk whats this man

    editorWindowScrollIfNeeded(window);

    window->backgroundColor = lerp(window->backgroundColor, DEFAULT_COLOR_BACKGROUND, 0.1);
    pushQuad(renderBufferBackground, {window->left + window->scrollX, window->top - window->scrollY, 0},
            {window->width, window->height}, uvs, window->backgroundColor);

    // current editing line
    pushQuad(renderBufferBackground, v3(window->left + window->scrollX, window->cursor.y, 0), {window->width, FONT_HEIGHT}, uvs, DEFAULT_COLOR_LINE);

    
    fontRenderGapBuffer({window->left, window->top}, &window->currentFile->buffer, renderBuffer, renderBufferUI, font, 
                        window->scrollTop, window->scrollBottom);

    for(int i = 0; i < window->selections.currentAmount; ++i){
        Selection selection = window->selections[i];
        if(selection.end < window->currentFile->buffer.cursor - 1024 || selection.start > window->currentFile->buffer.cursor + 1024){
            continue;
        }
        visualsRenderSelection({window->left, window->top}, selection, &window->currentFile->buffer, renderBuffer, renderBufferUI, font, SELECTION_SECONDARY_COLOR_TEXT);
    }

    if(window->temporarySelection.start != window->temporarySelection.end){
        window->temporaryColor.w = lerp(window->temporaryColor.w, 0, 0.1);
        visualsRenderSelection({window->left, window->top}, window->temporarySelection, &window->currentFile->buffer, renderBuffer, renderBufferUI, font, window->temporaryColor);
        if(window->temporaryColor.w < 0.1){
            window->temporarySelection.start = window->temporarySelection.end;
        }
    }

    visualsRenderSelection({window->left, window->top}, window->currentFile->buffer.selection, &window->currentFile->buffer, renderBuffer, renderBufferUI, font, SELECTION_COLOR_TEXT);
	visualsRenderMarks({window->left, window->top}, &window->currentFile->buffer, renderBuffer, renderBufferUI, font);

    if(activeWindow){
        if(time < 20 || window->currentlyTyping){
            // set cursor width depending on whatever we found in the function above about the cursor
            if(gapCharacterAtIndex(&window->currentFile->buffer, window->currentFile->buffer.cursor) == '\t'){
                pushQuad(renderBufferUI, window->cursor, {FONT_HEIGHT * 2, FONT_HEIGHT}, uvs, DEFAULT_COLOR_CURSOR);
            } else {
                pushQuad(renderBufferUI, window->cursor, {FONT_HEIGHT / 2, FONT_HEIGHT}, uvs, DEFAULT_COLOR_CURSOR);
            }
        }
    } else {
        v3 position = window->cursor;
        position.x += CURSOR_BORDER;
        position.y += CURSOR_BORDER;

        pushQuad(renderBufferUI, window->cursor, {FONT_HEIGHT / 2, FONT_HEIGHT}, uvs, DEFAULT_COLOR_CURSOR);
        pushQuad(renderBufferUI, position, {FONT_HEIGHT / 2 - CURSOR_BORDER * 2, FONT_HEIGHT - CURSOR_BORDER * 2}, uvs, DEFAULT_COLOR_LINE);
    }

    SHADER_SCOPE(shaderUI->programId, {
        shaderSetUniform4m(shaderUI->locations.matrixView, window->view);
        shaderSetUniform4m(shaderUI->locations.matrixTransform, window->transform);
        shaderSetUniform32u(shaderUI->locations.boundsLeft, window->left);
        shaderSetUniform32u(shaderUI->locations.boundsRight, window->left + window->width);
        shaderSetUniform32u(shaderUI->locations.boundsTop, window->top);
        shaderSetUniform32u(shaderUI->locations.boundsBottom, window->bottom);
        flushRenderBuffer(GL_TRIANGLES, renderBufferBackground);
    });

    SHADER_SCOPE(shader->programId, {
        shaderSetUniform4m(shader->locations.matrixView, window->view);
        shaderSetUniform4m(shader->locations.matrixTransform, window->transform);
        shaderSetUniform32u(shader->locations.boundsLeft, window->left);
        shaderSetUniform32u(shader->locations.boundsRight, window->left + window->width);
        shaderSetUniform32u(shader->locations.boundsTop, window->top);
        shaderSetUniform32u(shader->locations.boundsBottom, window->bottom);
        flushRenderBuffer(GL_TRIANGLES, renderBuffer);
    });

    SHADER_SCOPE(shaderUI->programId, {
        shaderSetUniform4m(shaderUI->locations.matrixView, window->view);
        shaderSetUniform4m(shaderUI->locations.matrixTransform, window->transform);
        shaderSetUniform32u(shaderUI->locations.boundsLeft, window->left);
        shaderSetUniform32u(shaderUI->locations.boundsRight, window->left + window->width);
        shaderSetUniform32u(shaderUI->locations.boundsTop, window->top);
        shaderSetUniform32u(shaderUI->locations.boundsBottom, window->bottom);
        flushRenderBuffer(GL_TRIANGLES, renderBufferUI);
    });

    // TODO(Sarmis) make the status bar not suck ?
    // TODO(Sarmis) the implementation of the status bar is questionale to me
    //              but it works, but the scrolling is affected to to laziness
    pushQuad(renderBufferBackground, window->statusBar.position, window->statusBar.size, uvs, DEFAULT_COLOR_STATUS_BAR_BACKGROUND);
    if(window->currentFile->filename.data){
        char buffer[256];
        sprintf(buffer, "%.20s ln %d", window->currentFile->filename.data, (u32)(window->cursor.y / FONT_HEIGHT) + 1);
        fontRender((u8*)buffer, strlen(buffer), v2(window->statusBar.position.x + window->statusBar.size.x / 2, window->statusBar.position.y + FONT_HEIGHT - FONT_HEIGHT / 16), renderBuffer, font, DEFAULT_COLOR_STATUS_BAR_TEXT_COLOR);
    }

    SHADER_SCOPE(shaderUI->programId, {
        shaderSetUniform4m(shaderUI->locations.matrixView, window->view);
        shaderSetUniform4m(shaderUI->locations.matrixTransform, window->transform);
        shaderSetUniform32u(shaderUI->locations.boundsLeft, window->left);
        shaderSetUniform32u(shaderUI->locations.boundsRight, window->left + window->width);
        shaderSetUniform32u(shaderUI->locations.boundsTop, window->top);
        shaderSetUniform32u(shaderUI->locations.boundsBottom, window->bottom);
        flushRenderBuffer(GL_TRIANGLES, renderBufferBackground);
    });

    SHADER_SCOPE(shader->programId, {
        shaderSetUniform4m(shader->locations.matrixView, window->view);
        shaderSetUniform4m(shader->locations.matrixTransform, window->transform);
        shaderSetUniform32u(shader->locations.boundsLeft, window->left);
        shaderSetUniform32u(shader->locations.boundsRight, window->left + window->width);
        shaderSetUniform32u(shader->locations.boundsTop, window->top);
        shaderSetUniform32u(shader->locations.boundsBottom, window->bottom);
        flushRenderBuffer(GL_TRIANGLES, renderBuffer);
    });


}























  
 