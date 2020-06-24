#pragma once

#include "general.h"
#include "math.h"
#include "window.h"
#include "gap_buffer.h"
#include "colors.h"

struct Panel {
    v3 position;
    v2 size;
    v3 cursor;
    i32 shakeTime;
    char* description;
    Buffer<char*> suggestions;
    GapBuffer buffer;
    bool (*action)(void*, void*);
    void (*tick)(void*);
};

void panelDefaultTick(void* data){}

Panel panelCreate(v3 position, v2 size, char* description){
	Panel result = {};
	
    result.tick = panelDefaultTick;
    result.shakeTime = 0;
	result.position = position;
	result.size = size;
	result.description = description;
	result.buffer = gapCreateEmpty();
	
    return result;
}

void panelDecideCursorPositionByGapBuffer(Panel* panel, FontGL* font){
    for(int i = 0; i < panel->buffer.cursor; ++i){
        i32 convertedOffset = UserToGap(panel->buffer.gap, i);
        if(convertedOffset >= panel->buffer.size - 1){
            break;
        }
        switch(panel->buffer.data[convertedOffset]){
            case 0:{
                }
                break;

            case '\t': {
                    panel->cursor.x += FONT_HEIGHT * 2;
                }
                break;

            case '\n': {
                    panel->cursor.y += FONT_HEIGHT;
                    panel->cursor.x = panel->position.x + 12;
                }
                break;

            default: {
                    Glyph glyph = font->glyphs[panel->buffer.data[convertedOffset] - ' '];
                    panel->cursor.x += glyph.xadvance;
                }
                break;
        }
    }
}

void panelRender(Panel* panel, EditorWindow* currentWindow,
                 Shader* shader, Shader* shaderUI,
                 RenderBuffer* renderBuffer, RenderBuffer* renderBufferUI, RenderBuffer* renderBufferBackground,
                 FontGL* font,
                 i32 time, u32 windowWidth, u32 windowHeight){
    v2 uvs[4] = {};

    if(panel->shakeTime){
        panel->position = lerp(panel->position, v3(currentWindow->left + sin(time * 0.6) * 20, currentWindow->top, 0), 0.3);
        --panel->shakeTime;
    } else {
        panel->position = lerp(panel->position, v3(currentWindow->left, currentWindow->top, 0), 0.3);
    }
    panel->cursor = {panel->position.x + 12, panel->position.y + 12 + FONT_HEIGHT + 4, 0};
    panelDecideCursorPositionByGapBuffer(panel, font);

    pushQuad(renderBufferBackground, panel->position, panel->size, uvs, DEFAULT_COLOR_PANEL_BACKGROUND);

    if(time < 10){
        pushQuad(renderBufferUI, panel->cursor, {FONT_HEIGHT / 2, FONT_HEIGHT + 3}, uvs, DEFAULT_COLOR_PANEL_CURSOR);
    }

    fontRenderGapBufferNoHighlights({panel->position.x + 12, panel->position.y + 12 + FONT_HEIGHT + 4}, &panel->buffer, renderBuffer, renderBufferUI, font, 0, FONT_HEIGHT * 4, DEFAULT_COLOR_PANEL_TEXT);
    fontRender((u8*)panel->description, strlen(panel->description), {panel->position.x + 12, panel->position.y + FONT_HEIGHT + 12}, renderBuffer, font, DEFAULT_COLOR_TEXT_PANEL_DESCRIPTION);

    r32 yOffset = (FONT_HEIGHT * 4);
    r32 xOffset = 0;
    for(int i = 0; i < panel->suggestions.currentAmount; ++i){
        r32 yAdjustment = 0;
        // ugly way to wrap the suggestions, its only one page
        if(yOffset + FONT_HEIGHT * i > currentWindow->top + currentWindow->height){
            yAdjustment = -currentWindow->height;
            xOffset = panel->size.x;
        } else {
            yAdjustment = 0;
        }
        v3 position = panel->position + v3(xOffset + 12, yAdjustment + yOffset + FONT_HEIGHT * i, 0);
        v2 size = v2(panel->size.x - 12, FONT_HEIGHT + 3);
        pushQuad(renderBufferBackground, position, size, uvs, DEFAULT_COLOR_PANEL_BACKGROUND_SUGGESTION);
        fontRender((u8*)panel->suggestions.array[i], strlen(panel->suggestions.array[i]), {position.x, position.y}, renderBuffer, font, DEFAULT_COLOR_TEXT_PANEL_SUGGESTION);
    }

    SHADER_SCOPE(shaderUI->programId, {
        shaderSetUniform4m(shaderUI->locations.matrixView, m4());
        shaderSetUniform4m(shaderUI->locations.matrixTransform, m4());
        shaderSetUniform32u(shaderUI->locations.boundsLeft, 0);
        shaderSetUniform32u(shaderUI->locations.boundsRight, windowWidth);
        shaderSetUniform32u(shaderUI->locations.boundsTop, 0);
        shaderSetUniform32u(shaderUI->locations.boundsBottom, windowHeight);
        flushRenderBuffer(GL_TRIANGLES, renderBufferBackground);
    });


    SHADER_SCOPE(shaderUI->programId, {
        shaderSetUniform4m(shaderUI->locations.matrixView, m4());
        shaderSetUniform4m(shaderUI->locations.matrixTransform, m4());
        shaderSetUniform32u(shaderUI->locations.boundsLeft, 0);
        shaderSetUniform32u(shaderUI->locations.boundsRight, windowWidth);
        shaderSetUniform32u(shaderUI->locations.boundsTop, 0);
        shaderSetUniform32u(shaderUI->locations.boundsBottom, windowHeight);
        flushRenderBuffer(GL_TRIANGLES, renderBufferUI);
    });

    SHADER_SCOPE(shader->programId, {
        shaderSetUniform4m(shader->locations.matrixView, m4());
        shaderSetUniform4m(shader->locations.matrixTransform, m4());
        shaderSetUniform32u(shader->locations.boundsLeft, 0);
        shaderSetUniform32u(shader->locations.boundsRight, windowWidth);
        shaderSetUniform32u(shader->locations.boundsTop, 0);
        shaderSetUniform32u(shader->locations.boundsBottom, windowHeight);
        flushRenderBuffer(GL_TRIANGLES, renderBuffer);
    });

}



































