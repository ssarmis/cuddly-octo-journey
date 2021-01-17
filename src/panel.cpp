#include "general.h"
#include "panel.h"
#include "app.h"

// TODO(Sarmis) add a panelKeyBindingsManger
bool panelDefaultTick(void* data0, void* data1){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*)data0;
    Panel* panel = &applicationLayoutData->panelGroup.panel;
    EditorWindow* currentWindow = applicationLayoutData->currentWindow;
    KeyboardManager* keyboardManager = (KeyboardManager*)data1;

    if(!(keyboardManager->currentActiveKeyStroke & KEY_CTRL || keyboardManager->currentActiveKeyStroke & KEY_CMD)){
        char potentialCharacter = keyboardManager->currentActiveKeyStroke & 0xff;
        if(isAlphanumericCharacter(potentialCharacter)){
            gapInsertCharacterAt(&panel->buffer, potentialCharacter, panel->buffer.cursor);
            gapIncreaseCursor(&panel->buffer);
            return true;
        } else if(isSpacingCharacter(potentialCharacter)){
            gapInsertCharacterAt(&panel->buffer, potentialCharacter, panel->buffer.cursor);
            gapIncreaseCursor(&panel->buffer);
            return true;
        } else if(keyboardManager->currentActiveKeyStroke & KEY_BACKSPACE){
            keyActionRemoveCharacterBeforeCursor(&panel->buffer);
            return true;
        } else {
            if(keyboardManager->currentActiveKeyStroke & KEY_RETURN){
                // TODO(Sarmis) panel->action(...)
                if(panel->suggestions.currentAmount){
                    String suggestion = panel->suggestions[panel->currentOption].name;
                    char* filename = gapToString(&panel->buffer);
                    String filenameString = cloneString(filename);
                    String directoryString;
                    directoryString.data = NULL;

                    u32 lastSlash = characterLastOccurence(filenameString, '/');
                    if(lastSlash){
                        ++lastSlash;
                        directoryString = subString(filenameString, 0, lastSlash + 1);
                    }

                    gapClean(&panel->buffer);

                    panel->buffer = gapCreateEmpty();
                    if(directoryString.data){
                        gapInsertNullTerminatedStringAt(&panel->buffer, (char*)directoryString.data, 0);
                        gapSeekCursor(&panel->buffer, directoryString.size);

                        gapInsertStringAt(&panel->buffer, suggestion, panel->buffer.cursor);
                    } else {
                        gapInsertStringAt(&panel->buffer, suggestion, 0);
                    }
                    gapSeekCursor(&panel->buffer, suggestion.size);
                }

                bool actionStatus = panel->action(applicationLayoutData);
                if(!actionStatus){
                    panel->shakeTime = 20;
                } else {
                    panel->active = false;
                    gapClean(&panel->buffer);
                }
            } else if(keyboardManager->currentActiveKeyStroke & KEY_DOWN){
                ++panel->currentOption;
                panel->currentOption = cuddle_clamp(panel->currentOption, 0, panel->suggestions.currentAmount - 1);
            } else if(keyboardManager->currentActiveKeyStroke & KEY_UP){
                --panel->currentOption;
                panel->currentOption = cuddle_clamp(panel->currentOption, 0, panel->suggestions.currentAmount - 1);
            }
        }
    }

    return false;
}

Panel panelCreate(v3 position, v2 size, char* description){
	Panel result = {};

    result.active = false;
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
        i32 convertedOffset = gapUserToGap (panel->buffer.gap, i);
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

void panelRenderSuggestions(Panel* panel, RenderBuffer* renderBuffer, FontGL* font){
    // TODO(Sarmis) precompute this shit
    // this doens't need to be in here, pretty
    // bad rendering but works for now
    r32 yOffset = (FONT_HEIGHT * 4);
    r32 xOffset = 12;
    i32 start = 0;
    i32 visibleLines = (panel->size.y - FONT_HEIGHT * 3) / FONT_HEIGHT - 1;
    r32 approxLine = panel->currentOption;

    while(approxLine >= start + visibleLines){
        ++start;
    }

    u32 linesToDisplay = panel->suggestions.currentAmount;
    linesToDisplay = cuddle_clamp(linesToDisplay, 0, visibleLines);

    for(int i = start; i < start + linesToDisplay; ++i){
        v3 position = panel->position + v3(xOffset, yOffset + FONT_HEIGHT * (i - start), 0);
        u32 stringSize = panel->suggestions[i].name.size;
        u32 charSize = FONT_HEIGHT / 2;
        u32 charsPerLine = panel->size.x / charSize;
        if(stringSize > charsPerLine - 7){
            stringSize = charsPerLine - 7;
        }
        
        v4 color = DEFAULT_COLOR_TEXT_PANEL_SUGGESTION;
        if(i == panel->currentOption){
            color = DEFAULT_COLOR_TEXT;
        }

        fontRender((u8*)panel->suggestions[i].name.data, stringSize, {position.x, position.y + 12}, renderBuffer, font, color);
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
        panel->position = lerp(panel->position, v3(currentWindow->left, currentWindow->top, 0), 0.5);
    }
    panel->cursor = {panel->position.x + 12, panel->position.y + 12 + FONT_HEIGHT + 4, 0};
    panelDecideCursorPositionByGapBuffer(panel, font);

    pushQuad(renderBufferBackground, panel->position, panel->size, uvs, DEFAULT_COLOR_PANEL_BACKGROUND);

    if(time < 10){
        pushQuad(renderBufferUI, panel->cursor, {FONT_HEIGHT / 2, FONT_HEIGHT + 3}, uvs, DEFAULT_COLOR_PANEL_CURSOR);
    }

    fontRenderGapBufferNoHighlights({panel->position.x + 12, panel->position.y + 12 + FONT_HEIGHT + 4}, &panel->buffer, renderBuffer, renderBufferUI, font, 0, FONT_HEIGHT * 4, DEFAULT_COLOR_PANEL_TEXT);
    fontRender((u8*)panel->description, strlen(panel->description), {panel->position.x + 12, panel->position.y + FONT_HEIGHT + 12}, renderBuffer, font, DEFAULT_COLOR_TEXT_PANEL_DESCRIPTION);

    panelRenderSuggestions(panel, renderBuffer, font);
    
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

 