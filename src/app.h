#pragma once

#include "gap_buffer.h"
#include "window.h"
#include "panel.h"
#include "panel_actions.h"
#include "panel_updates.h"
#include "editor_file_pool.h"

struct ApplicationLayoutData {
    bool scheduleChangeInSize;
    
    u32 windowWidth;
    u32 windowHeight;

    u32 windowCount;
    EditorWindow windows[2];
    EditorWindow layoutWindows[2];

    EditorFilePool filePool;

    u32 currentWindowIndex;
    EditorWindow* currentWindow;
    GapBuffer* currentBuffer;

    PanelGroup panelGroup;
};  
 