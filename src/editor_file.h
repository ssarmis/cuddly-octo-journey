#pragma once

#include "general.h"
#include "gap_buffer.h"

struct EditorFile {
    String filename;
    String fullPath;
    GapBuffer buffer;
};

EditorFile editorFileReadFile(char* filename){
    EditorFile result = {};

    String fullFilename = cloneString(filename);
    String file = {};

    i32 separator = icharacterLastOccurence(fullFilename, '/');

    if(separator != -1) {
        file = subString(fullFilename, separator + 2, fullFilename.size);
    } else {
        file = cloneString(filename);
    }

    result.filename = file;
    result.fullPath = fullFilename;
    result.buffer = gapReadFile((char*)fullFilename.data);

    return result;
}