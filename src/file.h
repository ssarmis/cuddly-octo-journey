#pragma once

#include <stdio.h>

#include "general.h"

struct File {
    u8* data;
    u32 size;
    u32 pointer;
};

static void skipWordInFile(File* file){
    while(file->pointer < file->size){
        if(file->data[file->pointer] == ' ' ||
           file->data[file->pointer] == '\n' ||
           file->data[file->pointer] == '\r'){
            break;
        }
        ++file->pointer;
    }
}

static void skipLineInFile(File* file){
    while(file->pointer < file->size){
        if(file->data[file->pointer] == '\n' ||
           file->data[file->pointer] == '\r'){
            break;
        }
        ++file->pointer;
    }
}

static void skipSpacesInFile(File* file){
    while(file->data[file->pointer] == ' ' || 
          file->data[file->pointer] == '\t'){
    
        ++file->pointer;
    }
}

static String readStringFromFile(File* file){
    String result = {};
    skipSpacesInFile(file);

    u8* start = &file->data[file->pointer];
    u32 pointerClone = file->pointer;

    while(!(file->data[file->pointer] == '\n' ||
            file->data[file->pointer] == '\r' ||
            file->data[file->pointer] == '\t' ||
            file->data[file->pointer] == ' ')){

        ++file->pointer;
    }

    result = cloneString(start, file->pointer - pointerClone);

    return result;
}

static u32 readU32FromFile(File* file){
    u32 result = 0;
    skipSpacesInFile(file);

    u8 character = file->data[file->pointer++];

    while((character >= '0' && character <= '9')) {
        result *= 10;
        result += character - '0';
        character = file->data[file->pointer++];
    }

    --file->pointer;

    return result;
}

static i32 readI32FromFile(File* file){
    i32 result = 0;
    skipSpacesInFile(file);

    u8 character = file->data[file->pointer++];
    bool negate = false;
    if(character == '-'){
        negate = true;
        character = file->data[file->pointer++];
    }

    while((character >= '0' && character <= '9')) {
        result *= 10;
        result += character - '0';
        character = file->data[file->pointer++];
    }

    if(negate){
        result *= -1;
    }

    --file->pointer;

    return result;
}

static r32 readR32FromFile(File* file){
    r32 result = 0;
    skipSpacesInFile(file);

    bool negate = file->data[file->pointer] == '-';
    if(negate){
        ++file->pointer;
    }

    u32 fractionalPartSize = 0;
    u8 character = file->data[file->pointer++];
    while((character >= '0' && character <= '9') || character == '.') {
        if(character == '.'){
            fractionalPartSize = 10;
        } else {
            if(fractionalPartSize){
                fractionalPartSize *= 10;
            }

            result *= 10;
            result += character - '0';
        }
        character = file->data[file->pointer++];
    }

    --file->pointer;
    fractionalPartSize /= 10;
    result /= fractionalPartSize;

    if(negate){
        result *= -1;
    }

    return result;
}

static inline File fileRead(const char* filename){
    File result = {};

    FILE* file = fopen(filename, "rb");

    ASSERT(file); // TODO(Sarmis) handle bad files differently, don't just crash

    fseek(file, 0, SEEK_END);
    result.size = ftell(file);
    fseek(file, 0, SEEK_SET);

    result.data = new u8[result.size];

    u32 readByte = 0;
    while(readByte = fread(result.data, sizeof(u8), result.size, file)){
    }

    fclose(file);

    return result;
}

static inline void fileClear(File* file){
    if(file->data){
        file->size = 0;
        file->pointer = 0;
        delete[] file->data;
    }
}