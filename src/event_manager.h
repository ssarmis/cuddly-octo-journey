#pragma once

#include "general.h"
#include <SDL2/SDL.h>
#include "keyboard_manager.h"

// SPONGE
// move definitions for keys in something else
#include "keyboard_bindings.h"

void eventTick(bool* done, KeyboardManager* keyboardManager){
    SDL_Event event;
    // TODO(Sarmis) make a macro for the mask
    // that clears everything except the CTRL, SHIFT and ALT
    keyboardManager->currentActiveKeyStroke = keyboardManager->currentActiveKeyStroke & ((~0) << 29);
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_WINDOWEVENT: {
                    switch(event.window.event){
                        case SDL_WINDOWEVENT_SIZE_CHANGED: {
                            }
                            break;
                    }
                }
                break;

            case SDL_KEYUP: {
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_LCTRL: case SDL_SCANCODE_RCTRL: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_CTRL;
                            }
                            break;

                        case SDL_SCANCODE_PAGEUP: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_PAGEUP;
                            }
                            break;

                        case SDL_SCANCODE_PAGEDOWN: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_PAGEDOWN;
                            }
                            break;

                        case SDL_SCANCODE_HOME: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_HOME;
                            }
                            break;

                        case SDL_SCANCODE_END: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_END;
                            }
                            break;

                        case SDL_SCANCODE_LSHIFT: case SDL_SCANCODE_RSHIFT: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_SHIFT;
                            }
                            break;

                        case SDL_SCANCODE_TAB: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_TAB;
                            }
                            break;

                        case SDL_SCANCODE_LEFT:{
                                keyboardManager->currentActiveKeyStroke &= ~KEY_LEFT;
                                // arrowReleased[3] = true;
                                // arrowTurbo = false;
                                // arrowTime = 0;
                            } 
                            break;

                        case SDL_SCANCODE_RIGHT: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_RIGHT;
                                // arrowReleased[2] = true;
                                // arrowTurbo = false;
                                // arrowTime = 0;
                            }
                            break;

                        case SDL_SCANCODE_UP:{
                                keyboardManager->currentActiveKeyStroke &= ~KEY_UP;
                                // arrowReleased[1] = true;
                                // arrowTurbo = false;
                                // arrowTime = 0;
                            }
                        
                        case SDL_SCANCODE_DOWN: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_DOWN;
                                // arrowReleased[0] = true;
                                // arrowTurbo = false;
                                // arrowTime = 0;
                            }
                            break;

                        case SDL_SCANCODE_DELETE: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_DELETE;
                            }
                            break;
                        
                         case SDL_SCANCODE_BACKSPACE: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_BACKSPACE;
                                // backspaceReleased = true;
                                // backspaceTurbo = false;
                                // backspaceTime = 0;
                            }
                            break;

                        case SDL_SCANCODE_RETURN2: case SDL_SCANCODE_RETURN: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_RETURN;
                                // newlineReleased = true;
                            }
                            break;
                    }
                }
                break;

            case SDL_KEYDOWN: {
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_ESCAPE: {
                            }
                            break;

                        case SDL_SCANCODE_LCTRL: case SDL_SCANCODE_RCTRL: {
                                keyboardManager->currentActiveKeyStroke |= KEY_CTRL;
                            }
                            break;

                        case SDL_SCANCODE_LSHIFT: case SDL_SCANCODE_RSHIFT: {
                                keyboardManager->currentActiveKeyStroke |= KEY_SHIFT;
                            }
                            break;

                        case SDL_SCANCODE_PAGEUP: {
                                keyboardManager->currentActiveKeyStroke |= KEY_PAGEUP;
                            }
                            break;

                        case SDL_SCANCODE_PAGEDOWN: {
                                keyboardManager->currentActiveKeyStroke |= KEY_PAGEDOWN;
                            }
                            break;

                        case SDL_SCANCODE_HOME: {
                                keyboardManager->currentActiveKeyStroke |= KEY_HOME;
                            }
                            break;

                        case SDL_SCANCODE_END: {
                                keyboardManager->currentActiveKeyStroke |= KEY_END;
                            }
                            break;

                        case SDL_SCANCODE_F12: {
                            }
                            break;

                        case SDL_SCANCODE_TAB: {
                                keyboardManager->currentActiveKeyStroke |= KEY_TAB;
                            }
                            break;

                        case SDL_SCANCODE_RETURN2: case SDL_SCANCODE_RETURN: {
                                keyboardManager->currentActiveKeyStroke |= KEY_RETURN;
                            }
                            break;

                        case SDL_SCANCODE_UP: {
                                keyboardManager->currentActiveKeyStroke |= KEY_UP;
                            }
                            break;

                        case SDL_SCANCODE_DOWN: {
                                keyboardManager->currentActiveKeyStroke |= KEY_DOWN;
                            }
                            break;

                        case SDL_SCANCODE_LEFT: {
                                keyboardManager->currentActiveKeyStroke |= KEY_LEFT;
                            }
                            break;

                        case SDL_SCANCODE_RIGHT: {
                                keyboardManager->currentActiveKeyStroke |= KEY_RIGHT;
                            }
                            break;

                        case SDL_SCANCODE_DELETE: {
                                keyboardManager->currentActiveKeyStroke |= KEY_DELETE;
                            }
                            break;

                        case SDL_SCANCODE_BACKSPACE: {
                                keyboardManager->currentActiveKeyStroke |= KEY_BACKSPACE;
                            }
                            break;
                            
                        default: {
                            // this must be in some sort of gneral manager for window buffer, 
                            // that will spawn panels when needed

                            char character = event.key.keysym.sym;
                            if(keyboardManager->currentActiveKeyStroke & KEY_SHIFT) {
                                character = keyboardManager->shiftCharactersLUT[character];
                            }
                            
                            keyboardManager->currentActiveKeyStroke |= (character & 0xff);

                            #if 0
                                // SPONGE
                                // tick at typing stroke
                                // well... yeah...about this function call...
                                panel.tick(&panel);

                                    currentBuffer->selection.end = currentBuffer->selection.start;
                                } else {
                                    switch (character) {
                                        case 'g':{
                                                panelActive = true;
                                                panel = gotoLinePanel;
                                                panel.buffer = gapCreateEmpty();
                                                panel.position.x = currentWindow->left;
                                                panel.position.y = -panel.size.y;
                                                currentBuffer = &panel.buffer;
                                            }
                                            break;
                                        case 'f': {
                                                panelActive = true;
                                                panel = findPanel;
                                                panel.buffer = gapCreateEmpty();
                                                panel.position.x = currentWindow->left;
                                                panel.position.y = -panel.size.y;
                                                currentBuffer = &panel.buffer;
                                            }
                                            break;

                                        case 'p': {
                                                panelActive = true;
                                                panel = openFilePanel;
                                                panel.buffer = gapCreateEmpty();
                                                panel.position.x = currentWindow->left;
                                                panel.position.y = -panel.size.y;
                                                currentBuffer = &panel.buffer;
                                            }
                                            break;

                                        case 's': {
                                                if(currentBuffer->dirty){
                                                    if(!currentBuffer->filename){
                                                        // file is not on disk
                                                        // this is the first save ever
                                                        // for this file...
                                                        panelActive = true;
                                                        panel = saveFilePanel;
                                                        panel.buffer = gapCreateEmpty();
                                                        panel.position.x = currentWindow->left;
                                                        panel.position.y = -panel.size.y;
                                                        currentBuffer = &panel.buffer;
                                                        // clear previous panel gap buffer is curenly opened
                                                    } else {
                                                        gapWriteFile(currentBuffer);
                                                        currentWindow->backgroundColor = {0, 0.1, 0};
                                                        TRACE("Saved fileasdasdasd %s\n", currentBuffer->filename);
                                                    }
                                                }
                                            }
                                            break;

                                        case 'v': {
                                                if(SDL_HasClipboardText()){
                                                    char* clipboard = SDL_GetClipboardText();
                                                    if(clipboard){
                                                        i32 distance = gapInsertNullTerminatedStringAt(currentBuffer, clipboard, currentBuffer->cursor);
                                                        gapSeekCursor(currentBuffer, distance);
                                                        SDL_free(clipboard);
                                                    }
                                                }
                                            }
                                            break;

                                        case 'c': {
                                                String copy = {};
                                                // TODO(Sarmis) solve case in which 
                                                // the gap in in the selection
                                                if(currentBuffer->selection.end < currentBuffer->gap.start){
                                                    copy = subString(currentBuffer->bufferString, currentBuffer->selection.start, currentBuffer->selection.end);
                                                } else {
                                                    copy = subString(currentBuffer->bufferString, UserToGap(currentBuffer->gap, currentBuffer->selection.start), UserToGap(currentBuffer->gap, currentBuffer->selection.end));
                                                }
                                                i32 status = SDL_SetClipboardText((char*)copy.data);
                                                if(status){
                                                    printf("SDL_SetClipboardText failed: %s\n", SDL_GetError());
                                                }
                                            }
                                            break;


                                        default:{
                                            }
                                            break;
                                    }
                                }
                                #endif
                            }
                            break;
                    }
                }
                break;

            case SDL_QUIT: {
                    *done = true;
                }
                break;
        }
    }
}





#if 0
                                // TODO(Sarmis) this results in segfault, needs bound checking
                                if(backspaceReleased || backspaceTurbo){
                                    if(gapGetSelectionSize(currentBuffer)){
                                        gapRemoveCharactersInRange(currentBuffer, currentBuffer->selection.start, currentBuffer->selection.end);
                                        gapSeekCursor(currentBuffer, -(currentBuffer->selection.end - currentBuffer->selection.start));
                                    } else {
                                        if(!controlSeeking){
                                            gapRemoveCharacterAt(currentBuffer, currentBuffer->cursor);
                                            gapDecreaseCursor(currentBuffer);
                                        } else {
                                            u32 end = currentBuffer->cursor;
                                            gapSeekCursorToPreviousCapitalOrSpace(currentBuffer);
                                            u32 start = currentBuffer->cursor + 1;
                                            gapRemoveCharactersInRange(currentBuffer, start, end);
                                        }
                                    }
                                } else {
                                    backspaceTime++;
                                    if (backspaceTime >= TURBO_TIME){
                                        backspaceTurbo = true;
                                    }
                                }
                                backspaceReleased = false;

                                // SPONGE
                                // tick at typing stroke
                                // well... yeah...about this function call...
                                panel.tick(&panel);

                                if(arrowReleased[2] || arrowTurbo){
                                    if(shiftPressed){
                                        if(!gapGetSelectionSize(currentBuffer)){
                                            // there is no current selection alive
                                            currentBuffer->selection.start = currentBuffer->cursor;
                                        }
                                    } else {
                                        currentBuffer->selection.start = currentBuffer->selection.end;
                                    }

                                    if(controlSeeking){
                                        gapSeekCursorToCapitalOrSpace(currentBuffer);
                                    } else {
                                        gapIncreaseCursor(currentBuffer);
                                    }

                                    if(shiftPressed){
                                        currentBuffer->selection.end = currentBuffer->cursor;
                                    }
                                } else {
                                    arrowTime++;
                                    if (arrowTime >= TURBO_TIME){
                                        arrowTurbo = true;
                                    }
                                }
                                arrowReleased[2] = false;
                                if(backspaceReleased || backspaceTurbo){
                                    if(gapGetSelectionSize(currentBuffer)){
                                        i32 distance = gapRemoveCharactersInRange(currentBuffer, currentBuffer->selection.start, currentBuffer->selection.end);
                                        gapSeekCursor(currentBuffer, -distance);
                                    } else {
                                        gapRemoveCharacterNearAt(currentBuffer, currentBuffer->cursor);
                                    }
                                } else {
                                    backspaceTime++;
                                    if (backspaceTime >= TURBO_TIME){
                                        backspaceTurbo = true;
                                    }
                                }
                                backspaceReleased = false;

                                // TODO(Sarmis) this doesn't really work properly
                                // did this quick to just have a basic thing ready
                                // for this thing...
                                if(arrowReleased[1] || arrowTurbo){
                                    i32 distance = gapGetDistanceFromPreviousNewline(currentBuffer);

                                    if(shiftPressed){
                                        if(!gapGetSelectionSize(currentBuffer)){
                                            // there is no current selection alive
                                            currentBuffer->selection.end = currentBuffer->cursor;
                                        }
                                    } else {
                                        currentBuffer->selection.start = currentBuffer->selection.end;
                                    }

                                    if(controlSeeking){
                                        gapSeekCursorINewlinesIfPossible(currentBuffer, -10);
                                    } else {
                                        gapSeekCursorToPreviousNewline(currentBuffer);
                                        if(currentBuffer->cursor - 1 >= 0){
                                            i32 distanceOnPreviousLine = gapGetDistanceFromPreviousNewline(currentBuffer);
                                            distanceOnPreviousLine -= distance;
                                            if(distanceOnPreviousLine >= 0){
                                                gapSeekCursor(currentBuffer, -distanceOnPreviousLine);
                                            }
                                        }
                                    }

                                    if(shiftPressed){
                                        currentBuffer->selection.start = currentBuffer->cursor;
                                    }
                                } else {
                                    arrowTime++;
                                    if (arrowTime >= TURBO_TIME){
                                        arrowTurbo = true;
                                    }
                                }
                                arrowReleased[1] = false;
                                if(arrowReleased[0] || arrowTurbo){
                                    i32 distance = gapGetDistanceFromPreviousNewline(currentBuffer);

                                    if(shiftPressed){
                                        if(!gapGetSelectionSize(currentBuffer)){
                                            // there is no current selection alive
                                            currentBuffer->selection.start = currentBuffer->cursor;
                                        }
                                    } else {
                                        currentBuffer->selection.start = currentBuffer->selection.end;
                                    }

                                    if(controlSeeking){
                                        gapSeekCursorINewlinesIfPossible(currentBuffer, 10);
                                    } else {
                                        // gapSeekCursorToNewline(currentBuffer);
                                        gapSeekCursorINewlinesIfPossible(currentBuffer, 2);

                                        // if(currentBuffer->cursor + 1 < currentBuffer->size-1){
                                        //     gapIncreaseCursor(currentBuffer);
                                        //     i32 lineLength = gapGetDistanceToNewline(currentBuffer);
                                        //     if(distance <= lineLength){
                                        //         gapSeekCursor(currentBuffer, distance - 1);
                                        //     } else {
                                        //         gapSeekCursor(currentBuffer, lineLength);
                                        //     }
                                        // }
                                    }

                                    if(shiftPressed){
                                        currentBuffer->selection.end = currentBuffer->cursor;
                                    }
                                } else {
                                    arrowTime++;
                                    if (arrowTime >= TURBO_TIME){
                                        arrowTurbo = true;
                                    }
                                }
                                arrowReleased[0] = false;
                                if(arrowReleased[3] || arrowTurbo){
                                    if(shiftPressed){
                                        if(!gapGetSelectionSize(currentBuffer)){
                                            // there is no current selection alive
                                            currentBuffer->selection.end = currentBuffer->cursor;
                                        }
                                    } else {
                                        currentBuffer->selection.start = currentBuffer->selection.end;
                                    }

                                    if(controlSeeking){
                                        gapSeekCursorToPreviousCapitalOrSpace(currentBuffer);
                                        i32 convertedCoordinate = UserToGap(currentBuffer->gap, currentBuffer->cursor);
                                        if(currentBuffer->data[convertedCoordinate] == '\n'){
                                            gapIncreaseCursor(currentBuffer);
                                        }
                                    } else {
                                        gapDecreaseCursor(currentBuffer);
                                    }

                                    if(shiftPressed){
                                        currentBuffer->selection.start = currentBuffer->cursor;
                                    }
                                } else {
                                    arrowTime++;
                                    if (arrowTime >= TURBO_TIME){
                                        arrowTurbo = true;
                                    }
                                }
                                arrowReleased[3] = false;


                                if(panelActive){
                                    panelActive = false;
                                    currentBuffer = &currentWindow->buffer;
                                }




                                u32 newWidth = event.window.data1;
                                u32 newHeight = event.window.data2;
                                // TODO(Sarmis) I have to fix these 
                                // depending on the new size,
                                // the already existing offset needs to 
                                // update

                                for(int i = 0; i < windowCount; ++i){
                                    r32 wr = (r32)windows[i].width / (r32)windowWidth;
                                    r32 hr = (r32)windows[i].height / (r32)windowHeight;

                                    windows[i].width = wr * newWidth;
                                    windows[i].height = hr * newHeight;

                                    windows[i].top = hr * windows[i].top;
                                    windows[i].bottom = windows[i].top + windows[i].height;

                                    windows[i].left = ((r32)windows[i].left / (r32)windowWidth) * newWidth;

                                    windows[i].scrollTop = windows[i].top;
                                    windows[i].scrollBottom = windows[i].bottom;
                                    windows[i].scrollLeft = windows[i].left;
                                    windows[i].scrollRight = windows[i].left + windows[i].width;

                                    windows[i].buffer.cursor = 0;
                                    
                                    windows[i].scrollY = 0;
                                    windows[i].scrollX = 0;
                                }

                                // this shouldn't be here...

                                SHADER_SCOPE(shader.programId, {
                                    shaderSetUniform4m(shader.locations.matrixPerspective, 
                                                    orthographic(0, newWidth, 0, newHeight));
                                });

                                SHADER_SCOPE(shaderUI.programId, {
                                    shaderSetUniform4m(shaderUI.locations.matrixPerspective, 
                                                    orthographic(0, newWidth, 0, newHeight));
                                });

                                windowWidth = newWidth;
                                windowHeight = newHeight;
                            }
                            break;

                                                            u8 newline = '\n';
                                if(newlineReleased){
                                    if(panelActive){
                                        
                                        bool panelActionStatus = panel.action(currentWindow, &panel.buffer);
                                        if(panelActionStatus){
                                            panelActive = false;
                                            currentBuffer = &currentWindow->buffer;
                                        } else {
                                            panel.shakeTime = 20;
                                        }
                                    } else {
                                        i32 tabs = gapGetAmontOfTabsBeforeCursor(currentBuffer);

                                        gapInsertCharacterAt(currentBuffer, newline, currentBuffer->cursor);
                                        gapIncreaseCursor(currentBuffer);

                                        while(tabs--){
                                            gapInsertCharacterAt(currentBuffer, '\t', currentBuffer->cursor);
                                            gapIncreaseCursor(currentBuffer);
                                        }
                                    }
                                }
                                newlineReleased = false;
                                u8 tab = '\t';
                                if(tabReleased){
                                    if(controlSeeking){
                                        ++currentWindowIndex;
                                        currentWindowIndex %= windowCount;
                                        currentWindow = &windows[currentWindowIndex];
                                        if(!panelActive){
                                            currentBuffer = &currentWindow->buffer;
                                        }
                                    } else {
                                        gapInsertCharacterAt(currentBuffer, tab, currentBuffer->cursor);
                                        gapIncreaseCursor(currentBuffer);
                                    }
                                }
                                tabReleased = false;
                                // TODO(Sarmis) this could end at a weird 
                                // location at the end of the buffer
                                // add bound checking
                                if(endReleased){
                                    if(shiftPressed){
                                        if(!gapGetSelectionSize(currentBuffer)){
                                            // there is no current selection alive
                                            currentBuffer->selection.start = currentBuffer->cursor;
                                        }
                                    } else {
                                        currentBuffer->selection.start = currentBuffer->selection.end;
                                    }

                                    i32 convertedCoordinate = UserToGap(currentBuffer->gap, currentBuffer->cursor);
                                    if(!(convertedCoordinate < 0 || convertedCoordinate > currentBuffer->size - 1)){
                                        if(currentBuffer->data[convertedCoordinate] != '\n'){
                                            gapSeekCursorToNewline(currentBuffer);
                                        }
                                    }

                                    if(shiftPressed){
                                        currentBuffer->selection.end = currentBuffer->cursor;
                                    }
                                }
                                endReleased = false;
                                if(homeReleased){
                                    if(shiftPressed){
                                        if(!gapGetSelectionSize(currentBuffer)){
                                            // there is no current selection alive
                                            currentBuffer->selection.end = currentBuffer->cursor;
                                        }
                                    } else {
                                        currentBuffer->selection.start = currentBuffer->selection.end;
                                    }

                                    gapSeekCursorToPreviousNewline(currentBuffer);

                                    if(currentBuffer->data[UserToGap(currentBuffer->gap, currentBuffer->cursor)] == '\n'){
                                        gapIncreaseCursor(currentBuffer);
                                    }

                                    if(shiftPressed){
                                        currentBuffer->selection.start = currentBuffer->cursor;
                                    }
                                }
                                homeReleased = false;
                                if(pageUpReleased){
                                    u32 heightInLines = windowHeight / FONT_HEIGHT;
                                    gapSeekCursorINewlinesIfPossible(currentBuffer, -heightInLines);
                                }
                                pageUpReleased = false;
                                // SPONGE 
                                // bug with the pagedown when resizing
                                // after scroll was done
                                if(pageDownReleased){
                                    u32 heightInLines = windowHeight / FONT_HEIGHT;
                                    gapSeekCursorINewlinesIfPossible(currentBuffer, heightInLines);
                                }
                                pageDownReleased = false;

#endif
