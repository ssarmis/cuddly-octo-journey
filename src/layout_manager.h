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

void layoutResizeWindows(LayoutEvent* layoutEvent, ApplicationLayoutData* applicationLayoutData){
    u32 newWidth = layoutEvent->data0;
    u32 newHeight = layoutEvent->data1;

    for(int i = 0; i < applicationLayoutData->windowCount; ++i){
        r32 wr = (r32)applicationLayoutData->windows[i].width / (r32)applicationLayoutData->windowWidth;
        r32 hr = (r32)applicationLayoutData->windows[i].height / (r32)applicationLayoutData->windowHeight;

        applicationLayoutData->windows[i].width = wr * newWidth;
        applicationLayoutData->windows[i].height = hr * newHeight;

        applicationLayoutData->windows[i].top = hr * applicationLayoutData->windows[i].top;
        applicationLayoutData->windows[i].bottom = applicationLayoutData->windows[i].top + applicationLayoutData->windows[i].height;

        applicationLayoutData->windows[i].left = ((r32)applicationLayoutData->windows[i].left / (r32)applicationLayoutData->windowWidth) * newWidth;

        applicationLayoutData->windows[i].scrollTop = applicationLayoutData->windows[i].top;
        applicationLayoutData->windows[i].scrollBottom = applicationLayoutData->windows[i].bottom;
        applicationLayoutData->windows[i].scrollLeft = applicationLayoutData->windows[i].left;
        applicationLayoutData->windows[i].scrollRight = applicationLayoutData->windows[i].left + applicationLayoutData->windows[i].width;

        applicationLayoutData->windows[i].buffer.cursor = 0;
        
        applicationLayoutData->windows[i].scrollY = 0;
        applicationLayoutData->windows[i].scrollX = 0;
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
    applicationLayoutData->panelGroup.panel.position.x = applicationLayoutData->currentWindow->left;
    applicationLayoutData->panelGroup.panel.position.y = -applicationLayoutData->panelGroup.panel.size.y;
}

void layoutKeyActionChangeCurrentWindowToNext(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    applicationLayoutData->currentWindowIndex++;
    applicationLayoutData->currentWindowIndex %= applicationLayoutData->windowCount;
    applicationLayoutData->currentWindow = &applicationLayoutData->windows[applicationLayoutData->currentWindowIndex];
    applicationLayoutData->currentBuffer = &applicationLayoutData->currentWindow->buffer;
}

void layoutKeyActionCloseActivePanel(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    if(applicationLayoutData->panelGroup.panel.active){
        applicationLayoutData->panelGroup.panel.active = false;
        gapClean(&applicationLayoutData->panelGroup.panel.buffer);
    }
    applicationLayoutData->currentBuffer = &applicationLayoutData->currentWindow->buffer;
}

void layoutKeyActionOpenFilePanel(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    if(applicationLayoutData->panelGroup.panel.active){
        gapClean(&applicationLayoutData->panelGroup.panel.buffer);
    }


    layoutInitializePanelToShow(applicationLayoutData, applicationLayoutData->panelGroup.openFilePanel);
    applicationLayoutData->currentBuffer = &applicationLayoutData->panelGroup.panel.buffer;
}

void layoutKeyActionSaveFilePanel(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    if(applicationLayoutData->currentWindow->buffer.filename){
        gapWriteFile(&applicationLayoutData->currentWindow->buffer);
        applicationLayoutData->currentWindow->backgroundColor = {0, 0.1, 0};
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
    applicationLayoutData->currentBuffer = &applicationLayoutData->panelGroup.panel.buffer;
}

void layoutKeyActionCopyString(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;
    String copy = {};
    
    // TODO(Sarmis) solve case in which 
    // the gap in in the selection
    i32 selectionSize = gapGetSelectionSize(applicationLayoutData->currentBuffer);

    i32 selectionBeggining;
    i32 selectionEnding;
    if(selectionSize > 0){
        selectionBeggining = UserToGap(applicationLayoutData->currentBuffer->gap, applicationLayoutData->currentBuffer->selection.start);
        selectionEnding = UserToGap(applicationLayoutData->currentBuffer->gap, applicationLayoutData->currentBuffer->selection.end);
    } else {
        selectionBeggining = UserToGap(applicationLayoutData->currentBuffer->gap, applicationLayoutData->currentBuffer->selection.end);
        selectionEnding = UserToGap(applicationLayoutData->currentBuffer->gap, applicationLayoutData->currentBuffer->selection.start);
    }

    copy = subString(applicationLayoutData->currentBuffer->bufferString, selectionBeggining, selectionEnding);

    i32 status = SDL_SetClipboardText((char*)copy.data);

    if(status){
        printf("SDL_SetClipboardText failed: %s\n", SDL_GetError());
    }

    if(copy.data){
        delete[] copy.data;
    }
}

void layoutKeyActionPasteString(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    if(SDL_HasClipboardText()){
        char* clipboard = SDL_GetClipboardText();
        if(clipboard){
            i32 distance = gapInsertNullTerminatedStringAt(applicationLayoutData->currentBuffer, clipboard, applicationLayoutData->currentBuffer->cursor);
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

	    i32 selectionBeggining;
	    i32 selectionEnding;
	    if(selectionSize > 0){
	        selectionBeggining = UserToGap(applicationLayoutData->currentBuffer->gap, applicationLayoutData->currentBuffer->selection.start);
	        selectionEnding = UserToGap(applicationLayoutData->currentBuffer->gap, applicationLayoutData->currentBuffer->selection.end);
	    } else {
	        selectionBeggining = UserToGap(applicationLayoutData->currentBuffer->gap, applicationLayoutData->currentBuffer->selection.end);
	        selectionEnding = UserToGap(applicationLayoutData->currentBuffer->gap, applicationLayoutData->currentBuffer->selection.start);
	    }

		gapRemoveCharactersInRange(applicationLayoutData->currentBuffer, selectionBeggining, selectionEnding);

		applicationLayoutData->currentBuffer->selection.end = applicationLayoutData->currentBuffer->selection.start;
	}
}

void layoutKeyBindingInitialize(KeyboardBindingManager* keyboardBindingManager){
    keyBindingAddEntry1(keyboardBindingManager, KEY_ESCAPE,         layoutKeyActionCloseActivePanel);

#ifdef __unix__
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'x',     layoutKeyActionCopyAndRemoveString);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'c',     layoutKeyActionCopyString);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'v',     layoutKeyActionPasteString);

    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'p',     layoutKeyActionOpenFilePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'o',     layoutKeyActionOpenFilePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 's',     layoutKeyActionSaveFilePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'g',     layoutKeyActionGotoLinePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'f',     layoutKeyActionFindPanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_TAB, layoutKeyActionChangeCurrentWindowToNext);
#elif defined __APPLE__
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'x',     layoutKeyActionCopyAndRemoveString);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'c',     layoutKeyActionCopyString);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'v',     layoutKeyActionPasteString);

    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'p',     layoutKeyActionOpenFilePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'o',     layoutKeyActionOpenFilePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 's',     layoutKeyActionSaveFilePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'g',     layoutKeyActionGotoLinePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CMD | 'f',     layoutKeyActionFindPanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_TAB, layoutKeyActionChangeCurrentWindowToNext);
#endif
}

