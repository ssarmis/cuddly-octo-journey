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

// NOTE(Sarmis) my lazyness is beyond this world
extern bool quickOpenFileTick(void*, void*);
extern bool quickOpenFileAction(void*);

extern bool openFileTick(void*, void*);
extern bool openFileAction(void*);

extern bool saveFileTick(void*, void*);
extern bool saveFileAction(void*);

extern bool findTick(void*, void*);
extern bool findAction(void*);

extern bool gotoLineTick(void*, void*);
extern bool gotoLineAction(void*);

static void applicationLayoutDataInitialize(ApplicationLayoutData* applicationLayoutData){
    applicationLayoutData->scheduleChangeInSize = false;

    applicationLayoutData->windowWidth = 1280;
    applicationLayoutData->windowHeight = 768;

    applicationLayoutData->windowCount = 2;
    applicationLayoutData->windows[0] = windowCreate(applicationLayoutData->windowWidth / 2,
                                                    applicationLayoutData->windowHeight, 0, 0);
    applicationLayoutData->windows[1] = windowCreate(applicationLayoutData->windowWidth / 2,
                                                    applicationLayoutData->windowHeight, applicationLayoutData->windowWidth / 2 + 2, 0);

    applicationLayoutData->layoutWindows[0] = applicationLayoutData->windows[0]; 
    applicationLayoutData->layoutWindows[1] = applicationLayoutData->windows[1];

    applicationLayoutData->currentWindowIndex = 0;
    applicationLayoutData->currentWindow = &applicationLayoutData->windows[0];
    applicationLayoutData->currentBuffer = &applicationLayoutData->currentWindow->currentFile->buffer;

    EditorFile* bufferFile[2];
    bufferFile[0] = new EditorFile();
    bufferFile[1] = new EditorFile();

    bufferFile[0]->buffer = gapCreateEmpty();
    bufferFile[0]->filename.data = NULL;
    bufferFile[0]->fullPath.data = NULL;

    bufferFile[1]->buffer = gapCreateEmpty();
    bufferFile[1]->filename.data = NULL;
    bufferFile[1]->fullPath.data = NULL;

    applicationLayoutData->windows[0].currentFile = bufferFile[0];
    applicationLayoutData->windows[1].currentFile = bufferFile[1];

    gapInsertNullTerminatedStringAt(&applicationLayoutData->windows[1].currentFile->buffer, R"(
********************************************************************

                    uwu Welcome to my editor

            I hope you won't get annoyed by the bugs ;)
        it has some basic functionalities:
            - basic C/C++ syntax highlighting (static, int, void, etc...)
              not customizeable yet without the source code
            - 2 windows, one can be closed, the other can be opened again
              with CTRL + SHIFT + UP,
              any number of windows is supported with any positioning
              but I don't have a proper grid system in place right now
            - basic UNDO
            - find/goto line
            - highlights on words

        Thank you for using me *chu* <3

********************************************************************

    )", 0);

    // TODO(Sarmis) make initialization for these
    // TODO(Sarmis) using virtual functions would yield
    //              a similar result but with les hussle

    // NOTE(Sarmis) the width and height of the panels are kind of
    // useless now, kind of hardcoded in the layout_manager anyway
    // for now
    applicationLayoutData->panelGroup.quickOpenPanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Quick open");
    applicationLayoutData->panelGroup.quickOpenPanel.action = quickOpenFileAction;
    applicationLayoutData->panelGroup.quickOpenPanel.tick = quickOpenFileTick;

    applicationLayoutData->panelGroup.openFilePanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Open file");
    applicationLayoutData->panelGroup.openFilePanel.action = openFileAction;
    applicationLayoutData->panelGroup.openFilePanel.tick = openFileTick;

    applicationLayoutData->panelGroup.findPanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Find");
    applicationLayoutData->panelGroup.findPanel.action = findAction;
    applicationLayoutData->panelGroup.findPanel.tick = findTick;

    applicationLayoutData->panelGroup.gotoLinePanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Goto line");
    applicationLayoutData->panelGroup.gotoLinePanel.action = gotoLineAction;

    applicationLayoutData->panelGroup.saveFilePanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Save file");
    applicationLayoutData->panelGroup.saveFilePanel.action = saveFileAction;
}
