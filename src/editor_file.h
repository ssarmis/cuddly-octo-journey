#pragma once

#include "general.h"
#include "gap_buffer.h"

struct EditorFile {
    String filename;
    String fullPath;
    GapBuffer buffer;
};

static EditorFile editorFileReadFile(char* filename){
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

static EditorFile* getEditorFileByFullPath(Buffer<EditorFile> buffer, String string){
    EditorFile* result = NULL;
    for(int i = 0; i < buffer.currentAmount; ++i){
        if(buffer[i].fullPath == string){
            return &buffer[i];
        }
    }

    return result;
}