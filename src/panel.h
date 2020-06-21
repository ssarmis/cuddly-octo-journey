#pragma once

#include "general.h"
#include "math.h"
#include "window.h"
#include "gap_buffer.h"

struct Panel {
    v3 position;
    v2 size;
    v3 cursor;
    char* description;
    GapBuffer buffer;
    void (*action)(void*, void*);
};

Panel panelCreate(v3 position, v2 size, char* description){
	Panel result = {};
	
	result.position = position;
	result.size = size;
	result.description = description;
	result.buffer = gapCreateEmpty();
	
    return result;
}

void panelDecideCursorPositionByGapBuffer(Panel* panel, FontGL* font){
    for(int i = 0; i < panel->buffer.cursor; ++i){
        switch(panel->buffer.data[UserToGap(panel->buffer.gap, i)]){
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
                    Glyph glyph = font->glyphs[panel->buffer.data[UserToGap(panel->buffer.gap, i)] - ' '];
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
                 int time, u32 windowWidth, u32 windowHeight){
    v2 uvs[4] = {};

    panel->position = lerp(panel->position, v3(currentWindow->left, currentWindow->top, 0), 0.3);
    panel->cursor = {panel->position.x + 12, panel->position.y + 12 + FONT_HEIGHT + 4, 0};
    panelDecideCursorPositionByGapBuffer(panel, font);

    pushQuad(renderBufferBackground, panel->position, panel->size, uvs, {0.2, 0.2, 0.2});

    if(time < 10){
        pushQuad(renderBufferUI, panel->cursor, {FONT_HEIGHT / 2, FONT_HEIGHT + 3}, uvs, v3(1, 1, 0));
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

    fontRenderGapBuffer({panel->position.x + 12, panel->position.y + 12 + FONT_HEIGHT + 4}, &panel->buffer, renderBuffer, renderBufferUI, font, 0, FONT_HEIGHT * 4);
    fontRender((u8*)panel->description, strlen(panel->description), {panel->position.x + 12, panel->position.y + FONT_HEIGHT + 12}, renderBuffer, font, {0.6, 0.6, 0.6});

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



































