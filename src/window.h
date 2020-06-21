#pragma once

#include "general.h"
#include "font.h"
#include "gap_buffer.h"
#include "math.h"

struct EditorWindow {
    i32 width;
    i32 height;

    r32 scrollY;
    r32 scrollX;
    r32 scrollTop;
    r32 scrollBottom;

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

    result.buffer = gapCreateEmpty();
    result.backgroundColor = DEFAULT_COLOR_BACKGROUND;

    return result;
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
    window->transform = translate({window->scrollX, window->scrollY, 0});

    if(window->cursor.y <= window->scrollTop){
        i32 distance = window->scrollTop - window->cursor.y;
        window->scrollBottom -= distance;
        window->scrollTop -= distance;
        window->scrollY += distance;
    } else if(window->cursor.y >= window->scrollBottom){
        i32 distance = window->cursor.y - window->scrollBottom + FONT_HEIGHT;
        window->scrollY -= distance;
        window->scrollBottom += distance;
        window->scrollTop += distance;
    }

    // current editing line
    pushQuad(renderBufferBackground, v3(window->left, window->cursor.y, 0), {window->width, FONT_HEIGHT + 3}, uvs, v3(0, 0, 0));

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
            pushQuad(renderBufferUI, window->cursor, {FONT_HEIGHT / 2, FONT_HEIGHT + 3}, uvs, v3(1, 1, 0));
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























