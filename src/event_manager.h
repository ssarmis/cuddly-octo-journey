#pragma once

#include "general.h"
#include <SDL2/SDL.h>
#include "keyboard_manager.h"

#include "layout_manager.h"

// SPONGE
// move definitions for keys in something else
#include "keyboard_bindings.h"

// TODO(Sarmis) maybe replace the applicationLayoutData thing with a WindowEvent
void eventTick(bool* done, LayoutEvent* layoutEvent, KeyboardManager* keyboardManager){
    SDL_Event event;
    // TODO(Sarmis) make a macro for the mask
    // that clears everything except the CTRL, SHIFT and ALT
    keyboardManager->currentActiveKeyStroke = keyboardManager->currentActiveKeyStroke & ((~0) << 28);
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_WINDOWEVENT: {
                    switch(event.window.event){
                        case SDL_WINDOWEVENT_SIZE_CHANGED: {
                                layoutEvent->data0 = event.window.data1;
                                layoutEvent->data1 = event.window.data2;
                                layoutEvent->type |= LAYOUT_EVENT_RESIZE;
                            }
                            break;
                    }
                }
                break;

            case SDL_KEYUP: {
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_ESCAPE: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_ESCAPE;
                            }
                            break;

                        case SDL_SCANCODE_LCTRL: case SDL_SCANCODE_RCTRL: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_CTRL;
                            }
                            break;

                        case SDL_SCANCODE_LGUI: case SDL_SCANCODE_RGUI: {
                                keyboardManager->currentActiveKeyStroke &= ~KEY_CMD;
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
                                keyboardManager->currentActiveKeyStroke |= KEY_ESCAPE;
                            }
                            break;

                        case SDL_SCANCODE_LCTRL: case SDL_SCANCODE_RCTRL: {
                                keyboardManager->currentActiveKeyStroke |= KEY_CTRL;
                            }
                            break;

                        case SDL_SCANCODE_LGUI: case SDL_SCANCODE_RGUI: {
                                keyboardManager->currentActiveKeyStroke |= KEY_CMD;
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
                                char character = event.key.keysym.sym;
                                if(keyboardManager->currentActiveKeyStroke & KEY_SHIFT) {
                                    character = keyboardManager->shiftCharactersLUT[character];
                                }

                                keyboardManager->currentActiveKeyStroke |= (character & 0xff);
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
