#pragma once

#include "general.h"
#include "math.h"
#include "window.h"
#include "gap_buffer.h"
#include "colors.h"

#include "keyboard_bindings.h"
#include "keyboard_manager.h"

struct Suggestion {
    String name;
    i32 score;
};

struct Panel {
    bool active;
    v3 position;
    v2 size;
    v3 cursor;
    i32 shakeTime;
    char* description;
    Buffer<Suggestion> suggestions;
    u32 lastFind;
    GapBuffer buffer;
    i32 currentOption;
    bool (*action)(void*);
    bool (*tick)(void*, void*);
};

struct PanelGroup {
    Panel panel; // the actual panel that will be ticked and displayed
    Panel quickOpenPanel;
    Panel openFilePanel;
    Panel findPanel;
    Panel saveFilePanel;
    Panel gotoLinePanel;
};


static void cleanSuggestionBuffer(Buffer<Suggestion> buffer){
    for(int i = 0; i < buffer.currentAmount; ++i){
        if(buffer[i].name.data){
            buffer[i].name.size = 0;
            delete[] buffer[i].name.data;
        }
    }
    
    bufferClean<Suggestion>(&buffer);
}


// TODO(Sarmis) add a panelKeyBindingsManger
bool panelDefaultTick(void* data0, void* data1);
Panel panelCreate(v3 position, v2 size, char* description);

void panelDecideCursorPositionByGapBuffer(Panel* panel, FontGL* font);

void panelRender(Panel* panel, EditorWindow* currentWindow,
                 Shader* shader, Shader* shaderUI,
                 RenderBuffer* renderBuffer, RenderBuffer* renderBufferUI, RenderBuffer* renderBufferBackground,
                 FontGL* font,
                 i32 time, u32 windowWidth, u32 windowHeight);
