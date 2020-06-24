#pragma once

#include "general.h"

#include "keyboard_manager.h"
#include "keyboard_bindings.h"

#include "panel.h"
#include "window.h"
#include "gap_buffer.h"

static KeyboardBindingManager layoutManagerKeybindings;

void layoutKeyActionOpenFilePanel(void* data0, void* data1){
    printf("asd\n");
    PanelGroup* panelGroup = (PanelGroup*) data0;
    EditorWindow* currentWindow = (EditorWindow*) data1;

    if(panelGroup->panel.active){
        gapClean(&panelGroup->panel.buffer);
    }

    // TODO(Sarmis) put this into a routine of its own
    panelGroup->panel = panelGroup->openFilePanel;
    panelGroup->panel.active = true;
    panelGroup->panel.buffer = gapCreateEmpty();
    panelGroup->panel.position.x = currentWindow->left;
    panelGroup->panel.position.y = -panelGroup->panel.size.y;
}

// TODO(Sarmis) parameters will change, probably one struct with stuff thats needed
void layoutManagerTick(PanelGroup* panelGroup, EditorWindow* currentWindow, KeyboardManager* keyboardManager){
    KeyboardBinding binding = keyBindingGetBindingByKey(&layoutManagerKeybindings, keyboardManager->currentActiveKeyStroke);
    if(!binding.key){
        return;
    }

    binding.keyAction2(panelGroup, currentWindow);
}

void layoutKeyBindingInitialize(KeyboardBindingManager* keyboardBindingManager){
    keyBindingAddEntry2(keyboardBindingManager, KEY_CTRL | 'p', layoutKeyActionOpenFilePanel);
}
