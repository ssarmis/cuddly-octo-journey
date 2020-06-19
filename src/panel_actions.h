#pragma once 

#include "window.h"
#include "panel.h"
#include "gap_buffer.h"

void openFileAction(void* data0, void* data1){
    EditorWindow* window = (EditorWindow*)data0;
    GapBuffer* filenameBuffer = (GapBuffer*)data1;
    char* filename = gapToString(filenameBuffer);

    window->buffer = gapReadFile(filename);

    window->scrollX = 0;
    window->scrollY = 0;
}