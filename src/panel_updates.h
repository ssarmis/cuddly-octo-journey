#pragma once

#include <unistd.h>
#include <dirent.h>

#include "general.h"

#include "panel.h"

bool stringIsPartiallyMatching(char* substring, char* string){
    while(*substring){
        if(*substring++ != *string++){
            return false;
        }
    }

    return true;
}

void openFileTick(void* data){
    // this should only update on keystrokes
    // this will be changed with the refactoring
    // this is an allocation party
    // reusing buffers would've been much better
    Panel* panel = (Panel*)data;
    char* filename = gapToString(&panel->buffer);

    String filenameString = cloneString(filename);
    String directoryString = cloneString(".");
    String fullpathString = filenameString;

    u32 lastSlash = characterLastOccurence(filenameString, '/');
    if(lastSlash){
        directoryString = subString(filenameString, 0, lastSlash);
        filenameString = subString(filenameString, lastSlash + 1, filenameString.size);
        fullpathString = directoryString + "/" + filenameString;
    }

    bufferClean(&panel->suggestions);

    DIR* directory = opendir((char*)directoryString.data);
    if (directory) {
        struct dirent* directoryEntry;
        while ((directoryEntry = readdir(directory)) != NULL) {
            if(stringIsPartiallyMatching((char*)filenameString.data, directoryEntry->d_name)){
                bufferAppend<char*>(&panel->suggestions, directoryEntry->d_name);
            }
        }
        closedir(directory);
    }

    if(filenameString.data != fullpathString.data){
        delete[] fullpathString.data;
        delete[] filenameString.data;
    } else {
        delete[] filenameString.data;
    }
    delete[] directoryString.data;
}