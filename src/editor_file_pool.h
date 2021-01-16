#pragma once

#include <unistd.h>
#include <dirent.h>

#include "general.h"
#include "editor_file.h"

struct EditorFilePool {
    Buffer<EditorFile> files;
};

static void editorFilePoolAppendPool(EditorFilePool* destination, EditorFilePool addition){
    bufferAppend<EditorFile>(&destination->files, &addition.files);
}
#include <stdio.h>
static EditorFilePool editorFilePoolLoadAllFilesFromDirectory(char* path, bool recursive=false){
    EditorFilePool result = {};

    Buffer<String> directories = {};

    DIR* directory = opendir(path);
    if (directory) {
        struct dirent* directoryEntry;
        while ((directoryEntry = readdir(directory)) != NULL) {
            String clone = cloneString(directoryEntry->d_name);
            if(clone.data[0] == '.' || clone == "CMakeFiles"){
                continue;
            }
            String pathClone = cloneString(path);
            clone = pathClone + "/" + clone;

            switch (directoryEntry->d_type) {
                case DT_DIR: {
                        bufferAppend<String>(&directories, clone);
                    }
                    break;
                
                case DT_REG: {
                        bufferAppend<EditorFile>(&result.files, editorFileReadFile((char*)clone.data));
                    }
                    break;

                default:
                    break;
            }

            delete[] pathClone.data;
        }
        closedir(directory);
    }

    for(int i = 0; i < directories.currentAmount; ++i){
        printf("%s %s\n", path, directories[i].data);
    }

    if(recursive){
        for(int i = 0; i < directories.currentAmount; ++i){
            printf("\n%s index %d max %d\n", path, i, directories.currentAmount);
            // printf("%s", directories[i].data);
            EditorFilePool additionalPool = editorFilePoolLoadAllFilesFromDirectory((char*)directories[i].data, true);
            editorFilePoolAppendPool(&result, additionalPool);
        }
    }

    cleanStringBuffer(directories);

    return result;
}

static void editorFilePoolFreeSpace(EditorFilePool* filePool){
    for(int i = 0; i < filePool->files.currentAmount; ++i){
        gapClean(&filePool->files[i].buffer);
        
        delete[] filePool->files[i].filename.data;
        filePool->files[i].filename.size = 0;

        delete[] filePool->files[i].fullPath.data;
        filePool->files[i].fullPath.size = 0;
    }
}