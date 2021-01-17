#pragma once

#include "general.h"

struct KeyboardManager {
    bool keys[256]; // TODO(Sarmis) remove this ?
    char shiftCharactersLUT[256];
    u32 currentActiveKeyStroke;
};

static void keyboardManagerInitialize(KeyboardManager* manager){
    for(char c = 'a'; c <= 'z'; ++c){
        manager->shiftCharactersLUT[c] = c - ' ';
    }

    manager->shiftCharactersLUT['1'] = '!';
    manager->shiftCharactersLUT['2'] = '@';
    manager->shiftCharactersLUT['3'] = '#';
    manager->shiftCharactersLUT['4'] = '$';
    manager->shiftCharactersLUT['5'] = '%';
    manager->shiftCharactersLUT['6'] = '^';
    manager->shiftCharactersLUT['7'] = '&';
    manager->shiftCharactersLUT['8'] = '*';
    manager->shiftCharactersLUT['9'] = '(';
    manager->shiftCharactersLUT['0'] = ')';
    manager->shiftCharactersLUT['['] = '{';
    manager->shiftCharactersLUT[']'] = '}';
    manager->shiftCharactersLUT['\''] = '"';
    manager->shiftCharactersLUT['\\'] = '|';
    manager->shiftCharactersLUT['/'] = '?';
    manager->shiftCharactersLUT[','] = '<';
    manager->shiftCharactersLUT['.'] = '>';
    manager->shiftCharactersLUT['='] = '+';
    manager->shiftCharactersLUT['-'] = '_';
    manager->shiftCharactersLUT[';'] = ':';
}