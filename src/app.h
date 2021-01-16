#pragma once

#include "gap_buffer.h"
#include "window.h"
#include "panel.h"
#include "editor_file_pool.h"

struct ApplicationLayoutData {
    bool scheduleChangeInSize;
    
    u32 windowWidth;
    u32 windowHeight;

    u32 windowCount;
    EditorWindow windows[2];
    EditorWindow layoutWindows[2];

    u32 currentWindowIndex;
    EditorWindow* currentWindow;
    GapBuffer* currentBuffer;

    EditorFilePool filePool;
    PanelGroup panelGroup;
};  
 