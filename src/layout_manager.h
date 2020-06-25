#pragma once

#include "general.h"

#include "keyboard_manager.h"
#include "keyboard_bindings.h"

#include "panel.h"
#include "window.h"
#include "gap_buffer.h"
#include "app.h"

static KeyboardBindingManager layoutManagerKeybindings;

void layoutKeyActionChangeCurrentWindowToNext(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    applicationLayoutData->currentWindowIndex++;
    applicationLayoutData->currentWindowIndex %= applicationLayoutData->windowCount;
    applicationLayoutData->currentWindow = &applicationLayoutData->windows[applicationLayoutData->currentWindowIndex];
}

void layoutKeyActionOpenFilePanel(void* data){
    ApplicationLayoutData* applicationLayoutData = (ApplicationLayoutData*) data;

    if(applicationLayoutData->panelGroup.panel.active){
        gapClean(&applicationLayoutData->panelGroup.panel.buffer);
    }

    // TODO(Sarmis) put this into a routine of its own
    applicationLayoutData->panelGroup.panel = applicationLayoutData->panelGroup.openFilePanel;
    applicationLayoutData->panelGroup.panel.active = true;
    applicationLayoutData->panelGroup.panel.buffer = gapCreateEmpty();
    applicationLayoutData->panelGroup.panel.position.x = applicationLayoutData->currentWindow->left;
    applicationLayoutData->panelGroup.panel.position.y = -applicationLayoutData->panelGroup.panel.size.y;
}

// TODO(Sarmis) parameters will change, probably one struct with stuff thats needed
void layoutManagerTick(ApplicationLayoutData* applicationLayoutData, KeyboardManager* keyboardManager){
    KeyboardBinding binding = keyBindingGetBindingByKey(&layoutManagerKeybindings, keyboardManager->currentActiveKeyStroke);
    if(!binding.key){
        return;
    }

    binding.keyAction1(applicationLayoutData);
}

void layoutKeyBindingInitialize(KeyboardBindingManager* keyboardBindingManager){
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | 'p', layoutKeyActionOpenFilePanel);
    keyBindingAddEntry1(keyboardBindingManager, KEY_CTRL | KEY_TAB, layoutKeyActionChangeCurrentWindowToNext);
}
