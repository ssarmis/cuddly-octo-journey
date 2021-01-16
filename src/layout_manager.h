#pragma once

#include "general.h"

#include "keyboard_manager.h"
#include "keyboard_bindings.h"

#include "panel.h"
#include "window.h"
#include "gap_buffer.h"
#include "app.h"

#define LAYOUT_EVENT_NONE    0
#define LAYOUT_EVENT_RESIZE  1

struct LayoutEvent {
    u32 type;
    // general purpose values
    i32 data0;
    i32 data1;
    i32 data2;
};

void layoutMoveWindow(EditorWindow* window, r32 newLeft, r32 newTop){
    window->top = newTop;
    window->bottom = window->top + window->height;

    window->left = newLeft;	
		
    window->scrollTop = window->top;
    window->scrollBottom = window->top + window->height;
    window->scrollLeft = window->left;
    window->scrollRight = window->left + window->width;

    // applicationLayoutData->windows[i].buffer.cursor = 0;
       
    window->scrollY = 0;
    window->scrollX = 0;

    window->statusBar.size = v2(window->width, STATUS_BAR_HEIGHT);
    window->statusBar.position = v3(window->left, window->height - STATUS_BAR_HEIGHT, 0);
}

void layoutResizeWindow(EditorWindow* window, u32 oldWidth, u32 oldHeight, u32 newWidth, u32 newHeight){
    r32 wr = (r32)window->width / (r32)oldWidth;
    r32 hr = (r32)window->height / (r32)oldHeight;

    window->width = wr * newWidth;
    window->height = hr * newHeight;

    window->top = hr * window->top;
    window->bottom = window->top + window->height;

    window->left = ((r32)window->left / (r32)oldWidth) * newWidth;	
		
    window->scrollTop = window->top;
    window->scrollBottom = window->top + window->height;
    window->scrollLeft = window->left;
    window->scrollRight = window->left + window->width;

    // applicationLayoutData->windows[i].buffer.cursor = 0;
       
    window->scrollY = 0;
    window->scrollX = 0;

    window->statusBar.size = v2(window->width, STATUS_BAR_HEIGHT);
    window->statusBar.position = v3(window->left, window->height - STATUS_BAR_HEIGHT, 0);
}

void layoutResizeWindows(LayoutEvent* layoutEvent, ApplicationLayoutData* applicationLayoutData){
    u32 newWidth = layoutEvent->data0;
    u32 newHeight = layoutEvent->data1;

    for(int i = 0; i < applicationLayoutData->windowCount; ++i){
		layoutResizeWindow(&applicationLayoutData->windows[i], applicationLayoutData->windowWidth, applicationLayoutData->windowHeight, newWidth, newHeight);	
		layoutResizeWindow(&applicationLayoutData->layoutWindows[i], applicationLayoutData->windowWidth, applicationLayoutData->windowHeight, newWidth, newHeight);	
	}

    applicationLayoutData->windowWidth = newWidth;
    applicationLayoutData->windowHeight = newHeight;

    applicationLayoutData->scheduleChangeInSize = true;
}

static KeyboardBindingManager layoutManagerKeybindings;

void layoutInitializePanelToShow(ApplicationLayoutData* applicationLayoutData, Panel panel){
    applicationLayoutData->panelGroup.panel = panel;
    applicationLayoutData->panelGroup.panel.active = true;
    applicationLayoutData->panelGroup.panel.buffer = gapCreateEmpty();
    applicationLayoutData->panelGroup.panel.size.x = applicationLayoutData->currentWindow->width;
    applicationLayoutData->panelGroup.panel.size.y = applicationLayoutData->currentWindow->height / 2;
    applicationLayoutData->panelGroup.panel.position.x = applicationLayoutData->currentWindow->left;
    applicationLayoutData->panelGroup.panel.position.y = -applicationLayoutData->panelGroup.panel.size.y;
}

void layoutKeyActionChangeCurrentWindowToNext(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    applicationLayoutData->currentWindowIndex++;
    applicationLayoutData->currentWindowIndex %= applicationLayoutData->windowCount;
    applicationLayoutData->currentWindow = &applicationLayoutData->windows[applicationLayoutData->currentWindowIndex];
    applicationLayoutData->currentBuffer = &applicationLayoutData->currentWindow->currentFile->buffer;
}

void layoutKeyActionCloseActivePanel(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    if(applicationLayoutData->panelGroup.panel.active){
        applicationLayoutData->panelGroup.panel.active = false;
        gapClean(&applicationLayoutData->panelGroup.panel.buffer);
    }
    applicationLayoutData->currentBuffer = &applicationLayoutData->currentWindow->currentFile->buffer;

    // reuse allocated space, don't deallocate
    applicationLayoutData->currentWindow->selections.currentAmount = 0;
}

void layoutKeyActionOpenFilePanel(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    if(applicationLayoutData->panelGroup.panel.active){
        gapClean(&applicationLayoutData->panelGroup.panel.buffer);
    }

    layoutInitializePanelToShow(applicationLayoutData, applicationLayoutData->panelGroup.openFilePanel);
    applicationLayoutData->currentBuffer = &applicationLayoutData->panelGroup.panel.buffer;
}

void layoutKeyActionQuickOpenFilePanel(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    if(applicationLayoutData->panelGroup.panel.active){
        gapClean(&applicationLayoutData->panelGroup.panel.buffer);
    }

    layoutInitializePanelToShow(applicationLayoutData, applicationLayoutData->panelGroup.quickOpenPanel);
    applicationLayoutData->currentBuffer = &applicationLayoutData->panelGroup.panel.buffer;
}

void layoutKeyActionSaveFilePanel(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    if(applicationLayoutData->currentWindow->currentFile->filename.data){
        gapWriteFile(&applicationLayoutData->currentWindow->currentFile->buffer);
        applicationLayoutData->currentWindow->backgroundColor = {0, 0.1, 0, 1};
    } else {
        if(applicationLayoutData->panelGroup.panel.active){
            gapClean(&applicationLayoutData->panelGroup.panel.buffer);
        }

        layoutInitializePanelToShow(applicationLayoutData, applicationLayoutData->panelGroup.saveFilePanel);
        applicationLayoutData->currentBuffer = &applicationLayoutData->panelGroup.panel.buffer;
    }
}
void layoutKeyActionGotoLinePanel(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    if(applicationLayoutData->panelGroup.panel.active){
        gapClean(&applicationLayoutData->panelGroup.panel.buffer);
    }

    layoutInitializePanelToShow(applicationLayoutData, applicationLayoutData->panelGroup.gotoLinePanel);
    applicationLayoutData->currentBuffer = &applicationLayoutData->panelGroup.panel.buffer;
}

void layoutKeyActionFindPanel(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    if(applicationLayoutData->panelGroup.panel.active){
        gapClean(&applicationLayoutData->panelGroup.panel.buffer);
    }

    
    layoutInitializePanelToShow(applicationLayoutData, applicationLayoutData->panelGroup.findPanel);
    applicationLayoutData->panelGroup.panel.size.y = FONT_HEIGHT * 3;
    applicationLayoutData->panelGroup.panel.lastFind = applicationLayoutData->currentWindow->currentFile->buffer.cursor;
    applicationLayoutData->currentBuffer = &applicationLayoutData->panelGroup.panel.buffer;
}

void layoutKeyActionCopyStringRange(void* data, i32 start, i32 end){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    String copy = {};
    copy = gapGetSubString(applicationLayoutData->currentBuffer, start, end);

    i32 status = SDL_SetClipboardText((char*)copy.data);

    if(status){
        printf("SDL_SetClipboardText failed: %s, thank you SDL...\n", SDL_GetError());
    }

    if(copy.data){
        delete[] copy.data;
    }
}

void layoutKeyActionCopyString(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;
    String copy = {};
    
    // TODO(Sarmis) solve case in which 
    // the gap in in the selection
    if(gapGetSelectionSize(applicationLayoutData->currentBuffer)){
        layoutKeyActionCopyStringRange(data, applicationLayoutData->currentBuffer->selection.start, applicationLayoutData->currentBuffer->selection.end);
    }
}

void layoutKeyActionPasteString(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    if(!applicationLayoutData->panelGroup.panel.active && gapGetSelectionSize(&applicationLayoutData->currentWindow->currentFile->buffer)){
        editorWindowKeyActionRemoveCharacterBeforeCursor(applicationLayoutData->currentWindow);
        applicationLayoutData->currentWindow->currentFile->buffer.selection.end = applicationLayoutData->currentWindow->currentFile->buffer.selection.start;
    }
    
    if(SDL_HasClipboardText()){
        char* clipboard = SDL_GetClipboardText();
        if(clipboard){
            i32 distance = gapInsertNullTerminatedStringAt(applicationLayoutData->currentBuffer, clipboard, applicationLayoutData->currentBuffer->cursor);
            // TODO(Sarmis) this will have issues when reverting stuff in panels
            if(!applicationLayoutData->panelGroup.panel.active){
                editorWindowAppendInsertAction(applicationLayoutData->currentWindow, applicationLayoutData->currentWindow->currentFile->buffer.cursor, applicationLayoutData->currentWindow->currentFile->buffer.cursor + distance);

                applicationLayoutData->currentWindow->temporaryColor = {1, 0, 0, 1};
                applicationLayoutData->currentWindow->temporarySelection = {applicationLayoutData->currentWindow->currentFile->buffer.cursor, applicationLayoutData->currentWindow->currentFile->buffer.cursor + distance};
            }

            gapSeekCursor(applicationLayoutData->currentBuffer, distance);
            SDL_free(clipboard);
        }
    }
}

// TODO(Sarmis) parameters will change, probably one struct with stuff thats needed
void layoutManagerTick(LayoutEvent* layoutEvent, ApplicationLayoutData* applicationLayoutData, KeyboardManager* keyboardManager){
    switch(layoutEvent->type){
        case LAYOUT_EVENT_NONE:{
            }
            break;

        case LAYOUT_EVENT_RESIZE:{
                layoutResizeWindows(layoutEvent, applicationLayoutData);
            }
            break;
    }

    layoutEvent->type = 0;
    layoutEvent->data0 = 0;
    layoutEvent->data1 = 0;
    layoutEvent->data2 = 0;

    KeyboardBinding binding = keyBindingGetBindingByKey(&layoutManagerKeybindings, keyboardManager->currentActiveKeyStroke);
    if(!binding.key){
        return;
    }

    binding.keyAction1(applicationLayoutData);
}

void layoutKeyActionCopyAndRemoveString(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

	i32 selectionSize = gapGetSelectionSize(applicationLayoutData->currentBuffer);

	if(selectionSize){
		layoutKeyActionCopyString(data);

        String removedString = gapGetSubString(&applicationLayoutData->currentWindow->currentFile->buffer, applicationLayoutData->currentWindow->currentFile->buffer.selection.start, applicationLayoutData->currentWindow->currentFile->buffer.selection.end);
        editorWindowAppendRemoveAction(applicationLayoutData->currentWindow, removedString, applicationLayoutData->currentWindow->currentFile->buffer.selection.start);

		gapRemoveCharactersInRange(applicationLayoutData->currentBuffer, applicationLayoutData->currentBuffer->selection.start, applicationLayoutData->currentBuffer->selection.end);

		applicationLayoutData->currentBuffer->selection.end = applicationLayoutData->currentBuffer->selection.start;
        applicationLayoutData->currentBuffer->cursor = applicationLayoutData->currentBuffer->selection.start;
	} else if(!applicationLayoutData->panelGroup.panel.active){
        editorWindowKeyActionMoveCursorToBegginingOfLine(applicationLayoutData->currentWindow);
        i32 start = applicationLayoutData->currentWindow->currentFile->buffer.cursor;
        editorWindowKeyActionMoveCursorToEndOfLine(applicationLayoutData->currentWindow);
        i32 end = applicationLayoutData->currentWindow->currentFile->buffer.cursor;


		layoutKeyActionCopyStringRange(data, start, end + 1);

        String removedString = gapGetSubString(&applicationLayoutData->currentWindow->currentFile->buffer, start, end + 1);
        editorWindowAppendRemoveAction(applicationLayoutData->currentWindow, removedString, start);

		gapRemoveCharactersInRange(applicationLayoutData->currentBuffer, start, end + 1);
        applicationLayoutData->currentWindow->currentFile->buffer.cursor = start;
    }
}

void layoutKeyActionMoveLinesUp(void* data){
    // ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

	// i32 selectionSize = gapGetSelectionSize(applicationLayoutData->currentBuffer);

	// if(selectionSize){
    //     // move selected lines(if there are more than one)
    //     i32 clone = applicationLayoutData->currentWindow->currentFile->buffer.cursor;

    //     applicationLayoutData->currentWindow->currentFile->buffer.cursor = applicationLayoutData->currentWindow->currentFile->buffer.gap.start;
    //     editorWindowKeyActionMoveCursorToBegginingOfLine(applicationLayoutData->currentWindow);
    //     i32 start = applicationLayoutData->currentWindow->currentFile->buffer.cursor;

    //     applicationLayoutData->currentWindow->currentFile->buffer.cursor = applicationLayoutData->currentWindow->currentFile->buffer.gap.end;
    //     editorWindowKeyActionMoveCursorToEndOfLine(applicationLayoutData->currentWindow);
    //     i32 end = applicationLayoutData->currentWindow->currentFile->buffer.cursor;

    //     applicationLayoutData->currentWindow->currentFile->buffer.cursor = start;

    //     gapSeekCursorToPreviousNewline(&applicationLayoutData->currentWindow->currentFile->buffer);
    //     gapSeekCursorToPreviousNewline(&applicationLayoutData->currentWindow->currentFile->buffer);

	// 	gapMoveRange(&applicationLayoutData->currentWindow->currentFile->buffer, start, end, applicationLayoutData->currentWindow->currentFile->buffer.cursor);
    // } else {
    //     // move only current line
    //     editorWindowKeyActionMoveCursorToBegginingOfLine(applicationLayoutData->currentWindow);
    //     i32 start = applicationLayoutData->currentWindow->currentFile->buffer.cursor;
    //     editorWindowKeyActionMoveCursorToEndOfLine(applicationLayoutData->currentWindow);
    //     i32 end = applicationLayoutData->currentWindow->currentFile->buffer.cursor;

    //     gapSeekCursorToPreviousNewline(&applicationLayoutData->currentWindow->currentFile->buffer);
    //     gapSeekCursorToPreviousNewline(&applicationLayoutData->currentWindow->currentFile->buffer);

	// 	gapMoveRange(&applicationLayoutData->currentWindow->currentFile->buffer, start, end, applicationLayoutData->currentWindow->currentFile->buffer.cursor);
    // }
}

void layoutKeyActionOpenNewBuffer(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    if(!applicationLayoutData->panelGroup.panel.active){
        gapClean(&applicationLayoutData->currentWindow->currentFile->buffer);
        applicationLayoutData->currentWindow->currentFile->buffer = gapCreateEmpty();
    }
}

void layoutKeyActionRestoreWindows(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

	for(int i = 0; i < applicationLayoutData->windowCount; ++i){
		// TODO(Sarmis) put these window properties in a struct
		// and just copy that one over, not manually	
		EditorWindow* window = &applicationLayoutData->windows[i];

		window->width = applicationLayoutData->layoutWindows[i].width;
		window->height = applicationLayoutData->layoutWindows[i].height;	

		window->left = applicationLayoutData->layoutWindows[i].left;
		window->top = applicationLayoutData->layoutWindows[i].top;
		window->bottom = applicationLayoutData->layoutWindows[i].bottom;

	    window->scrollTop = window->top;
	    window->scrollBottom = window->top + window->height;
	    window->scrollLeft = window->left;
	    window->scrollRight = window->left + window->width;

	    window->scrollY = 0;
	    window->scrollX = 0;

		window->visible = true;
		window->statusBar = applicationLayoutData->layoutWindows[i].statusBar;	
	}
}

void layoutKeyActionHideCurrentWindow(void* data){ 
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

	// TODO(Sarmis) implement something for the window layout
	// this is just meh
	u32 x = applicationLayoutData->currentWindow->left;
	u32 width = applicationLayoutData->currentWindow->width;

	applicationLayoutData->currentWindow->visible = false;

    applicationLayoutData->currentWindowIndex++;
    applicationLayoutData->currentWindowIndex %= applicationLayoutData->windowCount;
    applicationLayoutData->currentWindow = &applicationLayoutData->windows[applicationLayoutData->currentWindowIndex];
    applicationLayoutData->currentBuffer = &applicationLayoutData->currentWindow->currentFile->buffer;

	if(x > applicationLayoutData->currentWindow->left){
		layoutResizeWindow(applicationLayoutData->currentWindow, applicationLayoutData->currentWindow->width, applicationLayoutData->currentWindow->height, applicationLayoutData->currentWindow->width + width, applicationLayoutData->currentWindow->height);
	} else {
		layoutResizeWindow(applicationLayoutData->currentWindow, applicationLayoutData->currentWindow->width, applicationLayoutData->currentWindow->height, applicationLayoutData->currentWindow->width + width, applicationLayoutData->currentWindow->height);
		layoutMoveWindow(applicationLayoutData->currentWindow, x, applicationLayoutData->currentWindow->top);
	}
}

void layoutKeyActionSelectAll(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;
    if(!applicationLayoutData->panelGroup.panel.active){
        EditorWindow* window = applicationLayoutData->currentWindow;
        window->currentFile->buffer.selection.start = 0;
        window->currentFile->buffer.selection.end = gapGetAbstractSize(&window->currentFile->buffer) - 1;
    }
}

void layoutKeyActionUndoAction(void* data){
    // TODO(Sarmis) actions are per gap buffer btw, not per window
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;
    if(!applicationLayoutData->panelGroup.panel.active){
        EditorWindow* window = applicationLayoutData->currentWindow;

        if(window->actions.currentAmount){
            Action action = bufferPop<Action>(&window->actions);

            switch (action.type) {
                case ACTION_REMOVE:{
                        // reverse removing by inserting
                        gapInsertStringAt(&window->currentFile->buffer, action.data, action.location);
                        applicationLayoutData->currentWindow->currentFile->buffer.cursor = action.location + action.data.size;

                        applicationLayoutData->currentWindow->temporaryColor = {1, 0, 0, 1};
                        applicationLayoutData->currentWindow->temporarySelection = {action.location, action.location + action.data.size};

                        delete[] action.data.data; // just free the used data
                    }
                    break;

                case ACTION_INSERT:{
                        // reverse inserting by removing
                        gapRemoveCharactersInRange(&window->currentFile->buffer, action.start, action.end);
                        applicationLayoutData->currentWindow->currentFile->buffer.cursor = action.start;
                    }
                    break;
                
                default:
                    break;
            }
        }
    }
}

void layoutKeyBindingInitialize(KeyboardBindingManager* keyboardBindingManager){
    keyBindingAddEntry1(keyboardBindingManager, KEY_ESCAPE,         layoutKeyActionCloseActivePanel);

#ifdef __unix__
    keyBindingAddEntry1(keyboardBindingManager, KEY_ALT  | KEY_UP,  layoutKeyActionMoveLinesUp);

    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'w',     layoutKeyActionHideCurrentWindow);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_SHIFT | KEY_UP,     layoutKeyActionRestoreWindows);

    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'a',     layoutKeyActionSelectAll);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'z',     layoutKeyActionUndoAction);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'x',     layoutKeyActionCopyAndRemoveString);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'c',     layoutKeyActionCopyString);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'v',     layoutKeyActionPasteString);

    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'n',     layoutKeyActionOpenNewBuffer);

    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'p',     layoutKeyActionQuickOpenFilePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'o',     layoutKeyActionOpenFilePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 's',     layoutKeyActionSaveFilePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'g',     layoutKeyActionGotoLinePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'f',     layoutKeyActionFindPanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_TAB, layoutKeyActionChangeCurrentWindowToNext);
#elif defined __APPLE__
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'w',     layoutKeyActionHideCurrentWindow);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | KEY_SHIFT | KEY_UP,     layoutKeyActionRestoreWindows);

    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'a',     layoutKeyActionSelectAll);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'z',     layoutKeyActionUndoAction);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'x',     layoutKeyActionCopyAndRemoveString);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'c',     layoutKeyActionCopyString);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'v',     layoutKeyActionPasteString);

    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'p',     layoutKeyActionQuickOpenFilePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'o',     layoutKeyActionOpenFilePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 's',     layoutKeyActionSaveFilePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'g',     layoutKeyActionGotoLinePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'f',     layoutKeyActionFindPanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_TAB, layoutKeyActionChangeCurrentWindowToNext);
#endif
}

  
