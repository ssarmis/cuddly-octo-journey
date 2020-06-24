#pragma once

struct KeyboardManager {
    bool keys[256]; // TODO(Sarmis) remove this ?
    char shiftCharactersLUT[256];
    u32 currentActiveKeyStroke;
};