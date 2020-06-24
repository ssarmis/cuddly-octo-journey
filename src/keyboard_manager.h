#pragma once

struct KeyboardManager {
    bool keys[256];
    char shiftCharactersLUT[256];
    u32 currentActiveKeyStroke;
};