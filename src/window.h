#pragma once

#include "general.h"
#include "font.h"
#include "gap_buffer.h"
#include "math.h"

#include "keyboard_bindings.h"

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

    v3 backgroundColor;

    GapBuffer buffer;
};

EditorWindow windowCreate(i32 width, i32 height, u32 left, u32 top){
    EditorWindow result = {};

    result.width = width;
    result.height = height;

    result.cursor = v3(0);

    result.left = left;
    result.top = top;
    result.bottom = result.top + height;

    result.transform = m4();
    result.view = m4();

    result.scrollX = 0;
    result.scrollY = 0;

    result.scrollTop = result.top;
    result.scrollBottom = result.bottom;
    result.scrollLeft = result.left;
    result.scrollRight = result.left + result.width;

    result.buffer = gapCreateEmpty();
    result.backgroundColor = DEFAULT_COLOR_BACKGROUND;

    return result;
}

static KeyboardBindingManager windowKeyboardBindingManager;

#include "keyboard_manager.h"

bool isAlphanumericCharacter(){
    .
    .
    .
}

void editorWindowTick(EditorWindow* window, KeyboardManager* keyboardManager){
    // TODO(Sarmis) move the condition in a function like "isAlphanumericCharacter()"
    // also, make additional functions to see tabs and returns
    char potentialCharacter = keyboardManager->currentActiveKeyStroke & 0xff;
    if((potentialCharacter >= 'a' && potentialCharacter <= 'z') ||
       (potentialCharacter >= 'A' && potentialCharacter <= 'Z')){
           
        // TODO(Sarmis) well, this needs to be inserted into the buffer
        gapInsertCharacterAt(&window->buffer, potentialCharacter, window->buffer.cursor);
        gapIncreaseCursor(&window->buffer);
    } else {
        KeyboardBinding binding = keyBindingGetBindingByKey(&windowKeyboardBindingManager, keyboardManager->currentActiveKeyStroke);
        if(!binding.key){
           return;
        }

        binding.keyAction(&window->buffer);
    }
}

void editorWindowDecideCursorPositionByGapBuffer(EditorWindow* window, FontGL* font){
    for(int i = 0; i < window->buffer.cursor; ++i){
        i32 convertedOffset = UserToGap(window->buffer.gap, i);
        if(convertedOffset >= window->buffer.size - 1){
            break;
        }

        switch(window->buffer.data[convertedOffset]){
            case 0:{
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
                    Glyph glyph = font->glyphs[window->buffer.data[convertedOffset] - ' '];
                    window->cursor.x += glyph.xadvance;
                }
                break;
        }
    }
}

void editorWindowRender(EditorWindow* window, 
                        Shader* shader, Shader* shaderUI,
                        RenderBuffer* renderBuffer, RenderBuffer* renderBufferUI, RenderBuffer* renderBufferBackground,
                        FontGL* font,
                        int time, bool activeWindow){
    v2 uvs[4] = {};

    window->cursor = v3(window->left, window->top, 0);
    editorWindowDecideCursorPositionByGapBuffer(window, font);
    
    window->cursor.y += 3;
    window->transform = translate({-window->scrollX, window->scrollY, 0});

    if(window->cursor.x >= window->scrollRight){
        i32 distance = window->cursor.x - window->scrollRight + FONT_HEIGHT / 2;
        window->scrollRight += distance;
        window->scrollLeft += distance;
        window->scrollX += distance;
    } else if(window->cursor.x <= window->scrollLeft){
        i32 distance = window->scrollLeft - window->cursor.x;
        window->scrollRight -= distance;
        window->scrollLeft -= distance;
        window->scrollX -= distance;
    }

    if(window->cursor.y <= window->scrollTop){
        i32 distance = window->scrollTop - window->cursor.y;
        window->scrollBottom -= distance;
        window->scrollTop -= distance;
        window->scrollY += distance;
    } else if(window->cursor.y >= window->scrollBottom){
        i32 distance = window->cursor.y - window->scrollBottom + FONT_HEIGHT;
        window->scrollBottom += distance;
        window->scrollTop += distance;
        window->scrollY -= distance;
    }

    window->backgroundColor = lerp(window->backgroundColor, DEFAULT_COLOR_BACKGROUND, 0.1);
    pushQuad(renderBufferBackground, {window->left + window->scrollX, window->top - window->scrollY, 0},
            {window->width, window->height}, uvs, window->backgroundColor);

    // current editing line
    pushQuad(renderBufferBackground, v3(window->left + window->scrollX, window->cursor.y, 0), {window->width, FONT_HEIGHT + 3}, uvs, DEFAULT_COLOR_LINE);

    SHADER_SCOPE(shaderUI->programId, {
        shaderSetUniform4m(shaderUI->locations.matrixView, window->view);
        shaderSetUniform4m(shaderUI->locations.matrixTransform, window->transform);
        shaderSetUniform32u(shaderUI->locations.boundsLeft, window->left);
        shaderSetUniform32u(shaderUI->locations.boundsRight, window->left + window->width);
        shaderSetUniform32u(shaderUI->locations.boundsTop, window->top);
        shaderSetUniform32u(shaderUI->locations.boundsBottom, window->top + window->height);
        flushRenderBuffer(GL_TRIANGLES, renderBufferBackground);
    });

    fontRenderGapBuffer({window->left, window->top}, &window->buffer, renderBuffer, renderBufferUI, font, 
                        window->scrollTop - 40 * FONT_HEIGHT, window->scrollBottom + 40 * FONT_HEIGHT);

    if(activeWindow){
        if(time < 10){
            pushQuad(renderBufferUI, window->cursor, {FONT_HEIGHT / 2, FONT_HEIGHT + 3}, uvs, DEFAULT_COLOR_CURSOR);
        }
    }

    SHADER_SCOPE(shader->programId, {
        shaderSetUniform4m(shader->locations.matrixView, window->view);
        shaderSetUniform4m(shader->locations.matrixTransform, window->transform);
        shaderSetUniform32u(shader->locations.boundsLeft, window->left);
        shaderSetUniform32u(shader->locations.boundsRight, window->left + window->width);
        shaderSetUniform32u(shader->locations.boundsTop, window->top);
        shaderSetUniform32u(shader->locations.boundsBottom, window->top + window->height);
        flushRenderBuffer(GL_TRIANGLES, renderBuffer);
    });

    SHADER_SCOPE(shaderUI->programId, {
        shaderSetUniform4m(shaderUI->locations.matrixView, window->view);
        shaderSetUniform4m(shaderUI->locations.matrixTransform, window->transform);
        shaderSetUniform32u(shaderUI->locations.boundsLeft, window->left);
        shaderSetUniform32u(shaderUI->locations.boundsRight, window->left + window->width);
        shaderSetUniform32u(shaderUI->locations.boundsTop, window->top);
        shaderSetUniform32u(shaderUI->locations.boundsBottom, window->top + window->height);
        flushRenderBuffer(GL_TRIANGLES, renderBufferUI);
    });
}























