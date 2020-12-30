#pragma once

#include "general.h"

#define GAP_DEFAULT_SIZE 4

#define UserToGap(gap, x) (((x) < gap.start) ? (x) :( (gap.end - gap.start) + (x)))

struct Gap {
    i32 start;
    i32 end;
};

struct Selection {
    i32 start;
    i32 end;
};

struct GapBuffer {
    bool dirty;
    char* filename;
    union {
        String bufferString;
        struct {
            u8* data;
            i32 size;
        };
    };
    i32 cursor;
    Gap gap;
    Selection selection;
};

i32 gapGetGapSize(GapBuffer* buffer){
    return (buffer->gap.end - buffer->gap.start);
}

// #ifdef DEBUG_BUILD
void gapPrintGap(GapBuffer* buffer){
    for(int i = 0; i < buffer->size; ++i){
        if(i >= buffer->gap.start && i < buffer->gap.end){
            printf("_");
            continue;
        }
        if(!buffer->data[i]){
            printf("@");
            continue;
        }
        printf("%c", buffer->data[i]);
    }
    printf("\n");
    printf("gap size = %d\n", gapGetGapSize(buffer));
}
// #endif

void gapInitializeGap(GapBuffer* buffer, i32 position, i32 size){
    buffer->gap.start = position;
    buffer->gap.end = buffer->gap.start + size;
    // TODO(Sarmis) add assertions for when outside buffer
}

void gapGrowGap(GapBuffer* buffer, i32 size){
    u8* clone = buffer->data;
    
    buffer->data = new u8[buffer->size + size];
    memset(buffer->data, 0, buffer->size + size);

    for(int i = 0; i < buffer->gap.start; ++i){
        buffer->data[i] = clone[i];
    }

    for(int i = buffer->gap.end; i < buffer->size; ++i){
        buffer->data[i + size] = clone[i];
    }

    buffer->gap.end += size;

    buffer->size += size;

    delete[] clone;
}

void gapMoveCursor(GapBuffer* buffer, i32 position){
    if(position < 0 || position > buffer->size - 1){
        return;
    }

    buffer->cursor = position;
}

void gapMoveGap(GapBuffer* buffer, i32 position){
    if(position == buffer->gap.start){
        return;
    }
    
    if(position < buffer->gap.start){
        u32 shiftAmount = buffer->gap.start - position;
        while(shiftAmount--){
            // NOTE(Sarmis) I know that this is executed quite a few time
            // and doesn't look fast, but I will change it once
            // it becomes slow
            if(buffer->gap.start - 1 < 0 ||
               buffer->gap.end - 1 < 0){
                return;
            }
            buffer->data[buffer->gap.end-- - 1] = buffer->data[buffer->gap.start-- - 1];
        }
    } else if(position > buffer->gap.end){
        u32 shiftAmount = position - buffer->gap.end;
        while(shiftAmount--){
            // NOTE(Sarmis) I know that this is executed quite a few time
            // and doesn't look fast, but I will change it once
            // it becomes slow
            if(buffer->gap.start + 1 > buffer->size ||
               buffer->gap.end + 1 > buffer->size){
                return;
            }
            buffer->data[buffer->gap.start++] = buffer->data[buffer->gap.end++];
        }
    }
}

void gapReplaceCharacterAt(GapBuffer* buffer, char character, u32 position){
    if(position < 0 || position > buffer->size - 1){
        return;
    }
    buffer->data[position] = character;
}

void gapShrinkGap(GapBuffer* buffer){
    if(buffer->gap.start != buffer->gap.end){
        ++buffer->gap.start;
    }
}

void gapExtendGap(GapBuffer* buffer, i32 amount){
    buffer->gap.start -= amount;
    buffer->gap.start = clamp(buffer->gap.start, 0, buffer->size - 1);
}

void gapExtendGap(GapBuffer* buffer){
    gapExtendGap(buffer, 1);
}

void gapExtendGapBackwards(GapBuffer* buffer){
    ++buffer->gap.end;
    buffer->gap.end = clamp(buffer->gap.end, 0, buffer->size);
}

void gapInsertCharacterAt(GapBuffer* buffer, char character, i32 position){
    buffer->dirty = true;
    position = UserToGap(buffer->gap, position);
    gapMoveGap(buffer, position);

    gapReplaceCharacterAt(buffer, character, buffer->gap.start);
    gapShrinkGap(buffer); // basically ++gap.start

    if(!gapGetGapSize(buffer)){
        gapGrowGap(buffer, GAP_DEFAULT_SIZE);
    }
}

void gapRemoveCharacterNearAt(GapBuffer* buffer, i32 position){
    buffer->dirty = true;
    if(gapGetGapSize(buffer) + position > buffer->size){
        return;
    }
    gapMoveGap(buffer, UserToGap(buffer->gap, position));
    gapExtendGapBackwards(buffer); // basically ++gap.end
}

void gapRemoveCharacterAt(GapBuffer* buffer, i32 position){
    buffer->dirty = true;
    gapMoveGap(buffer, UserToGap(buffer->gap, position));
    gapExtendGap(buffer); // basically --gap.start
}

bool gapPointInGap(GapBuffer* buffer, i32 point){
    return (point >= buffer->gap.start && point < buffer->gap.end);
}

bool gapCursorInGap(GapBuffer* buffer){
    return (buffer->cursor >= buffer->gap.start && buffer->cursor <= buffer->gap.end);
}

void gapIncreaseCursor(GapBuffer* buffer){
    ++buffer->cursor;
    buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
}

void gapDecreaseCursor(GapBuffer* buffer){
    --buffer->cursor;
    buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
}

void gapSeekCursor(GapBuffer* buffer, i32 distance){
    buffer->cursor += distance;
    buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
}

i32 gapGetDistanceToNewline(GapBuffer* buffer){
    i32 clone = buffer->cursor;
    i32 convertedCursor = UserToGap(buffer->gap, clone);

    // TODO(Sarmis) since the end of the buffer might me bagic
    // just let this here for now..
    while(buffer->data[convertedCursor] != '\n' &&
          buffer->data[convertedCursor] != '\0'){
        convertedCursor = UserToGap(buffer->gap, clone + 1);
        if(convertedCursor < 0 || convertedCursor > buffer->size - 1){
            break;
        }
        ++clone;
    }
    clone = clamp(clone, 0, buffer->size - 1);
    return (clone - buffer->cursor);
}

i32 gapGetDistanceFromPreviousNewline(GapBuffer* buffer){
    i32 clone = buffer->cursor;
    i32 convertedCursor = UserToGap(buffer->gap, clone);
    if(convertedCursor < 0 || convertedCursor > buffer->size - 1){
        clone = clamp(clone, 0, buffer->size - 1);
        return (buffer->cursor - clone);
    }

    while(buffer->data[convertedCursor] != '\n'){
        convertedCursor = UserToGap(buffer->gap, clone - 1);
        if(convertedCursor < 0 || convertedCursor > buffer->size - 1){
            break;
        }
        --clone;
    }

    clone = clamp(clone, 0, buffer->size - 1);
    return (buffer->cursor - clone);
}

void gapSeekCursorToPreviousTabOrNewline(GapBuffer* buffer){
    gapDecreaseCursor(buffer);
    i32 convertedCursor = UserToGap(buffer->gap, buffer->cursor);

    while(buffer->data[convertedCursor] != '\t' &&
          buffer->data[convertedCursor] != '\n'){
        gapDecreaseCursor(buffer);
        convertedCursor = UserToGap(buffer->gap, buffer->cursor);
        if(convertedCursor < 0 || convertedCursor > buffer->size - 1){
            break;
        }
    }
    buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
}

void gapSeekCursorToPreviousNewline(GapBuffer* buffer){
    gapDecreaseCursor(buffer);
    i32 convertedCursor = UserToGap(buffer->gap, buffer->cursor);
    if(!convertedCursor || convertedCursor > buffer->size - 1){
        return;
    }

    while(buffer->data[convertedCursor] != '\n'){
        gapDecreaseCursor(buffer);
        convertedCursor = UserToGap(buffer->gap, buffer->cursor);
        if(!convertedCursor){
            return;
        } else if(convertedCursor >= buffer->size - 1){
            buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
            return;
        }
    }
}

void gapSeekCursorToNewline(GapBuffer* buffer){
	gapIncreaseCursor(buffer);
    i32 convertedCursor = UserToGap(buffer->gap, buffer->cursor);
    if(convertedCursor > buffer->size - 1){
        return;
    }

    // TODO(Sarmis) since the end of the buffer might me bagic
    // just let this here for now..
    while(buffer->data[convertedCursor] != '\n'){
        gapIncreaseCursor(buffer);
        convertedCursor = UserToGap(buffer->gap, buffer->cursor);
        if(convertedCursor > buffer->size - 1){
            return;
        }
    }
}

i32 gapGetAmontOfTabsBeforeCursor(GapBuffer* buffer){
    i32 result = 0;

    i32 clone = buffer->cursor;
    i32 convertedCursor = UserToGap(buffer->gap, --clone);

    while(buffer->data[convertedCursor] != '\n'){
        if(buffer->data[convertedCursor] == '\t'){
            ++result;
        }

        --clone;
        convertedCursor = UserToGap(buffer->gap, clone);
        if(convertedCursor < 0 || convertedCursor > buffer->size - 1){
            break;
        }
    }

    return result;
}

void gapSeekCursorToLine(GapBuffer* buffer, i32 line){
    // solution before line chaching
    u32 seekLocation = 0;
    for(int i = 0; i < buffer->size && line; ++i){
        if(i >= buffer->gap.start && i < buffer->gap.end){
            continue;
        }
        if(buffer->data[i] == '\n'){
            line--;
        }
        ++seekLocation;
    }

    buffer->cursor = seekLocation - 1;
    buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
}

i32 gapGetConsecutiveSpaces(GapBuffer* buffer){
    i32 clone = buffer->cursor;

    i32 convertedCursor = UserToGap(buffer->gap, clone + 1);

    // TODO(Sarmis) since the end of the buffer might me bagic
    // just let this here for now..
    while((buffer->data[convertedCursor] == ' '  ||
          buffer->data[convertedCursor] == '\t') &&
          buffer->data[convertedCursor] != '\0'){
        convertedCursor = UserToGap(buffer->gap, clone + 1);
        if(convertedCursor < 0 && convertedCursor > buffer->size - 1){
            break;
        }
        ++clone;
    }
    return (clone - buffer->cursor);
}

i32 gapGetPreviousConsecutiveSpaces(GapBuffer* buffer){
    i32 clone = buffer->cursor;

    i32 convertedCursor = UserToGap(buffer->gap, clone - 1);
    if(convertedCursor < 0 || convertedCursor > buffer->size - 1){
        clone = clamp(clone, 0, buffer->size - 1);
        return (buffer->cursor - clone);
    }
    --clone;

    // TODO(Sarmis) since the end of the buffer might me bagic
    // just let this here for now..
    while((buffer->data[convertedCursor] == ' '  ||
          buffer->data[convertedCursor] == '\t') &&
          buffer->data[convertedCursor] != '\0'){
        convertedCursor = UserToGap(buffer->gap, clone - 1);
        if(convertedCursor < 0 || convertedCursor > buffer->size - 1){
            break;
        }
        --clone;
    }

    clone = clamp(clone, 0, buffer->size - 1);
    return (buffer->cursor - clone);
}

void gapSeekCursorToCapitalOrSpace(GapBuffer* buffer){
    i32 possibleSkip = gapGetConsecutiveSpaces(buffer);
    if(possibleSkip > 1){
        buffer->cursor += possibleSkip;
        return;
    }

    gapIncreaseCursor(buffer);
    i32 convertedCursor = UserToGap(buffer->gap, buffer->cursor);
    if(convertedCursor < 0 || convertedCursor > buffer->size - 1){
        buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
        return;
    }

    // TODO(Sarmis) since the end of the buffer might me bagic
    // just let this here for now..
    while(buffer->data[convertedCursor] != '\n' && buffer->data[convertedCursor] != '"' &&
          buffer->data[convertedCursor] != ' ' && buffer->data[convertedCursor] != '!' &&
          buffer->data[convertedCursor] != '@' && buffer->data[convertedCursor] != '#' &&
          buffer->data[convertedCursor] != '$' && buffer->data[convertedCursor] != '%' &&
          buffer->data[convertedCursor] != '^' && buffer->data[convertedCursor] != '&' &&
          buffer->data[convertedCursor] != '*' && buffer->data[convertedCursor] != '(' &&
          buffer->data[convertedCursor] != '}' && buffer->data[convertedCursor] != '[' &&
          buffer->data[convertedCursor] != ']' && buffer->data[convertedCursor] != '<' &&
          buffer->data[convertedCursor] != '>' && buffer->data[convertedCursor] != '.' &&
          buffer->data[convertedCursor] != ',' && buffer->data[convertedCursor] != '/' &&
          buffer->data[convertedCursor] != '?' && buffer->data[convertedCursor] != '\\' &&
          !(buffer->data[convertedCursor] >= 'A' && buffer->data[convertedCursor] <= 'Z') &&
          buffer->data[convertedCursor] != '\0'){
        gapIncreaseCursor(buffer);
        convertedCursor = UserToGap(buffer->gap, buffer->cursor);
        if(convertedCursor < 0 || convertedCursor > buffer->size - 1){
            break;
        }
    }
    buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
}

void gapSeekCursorToPreviousCapitalOrSpace(GapBuffer* buffer){
    i32 possibleSkip = gapGetPreviousConsecutiveSpaces(buffer);
    if(possibleSkip > 1){
        gapSeekCursor(buffer, -possibleSkip);
        return;
    }
    
    gapDecreaseCursor(buffer);
    i32 convertedCursor = UserToGap(buffer->gap, buffer->cursor);
    if(convertedCursor < 0 || convertedCursor > buffer->size - 1){
        buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
        return;
    }
    // Ok for real I will make a function to see if its a symbol instead
    // of untolling it
    while(buffer->data[convertedCursor] != '\n' && buffer->data[convertedCursor] != '"' &&
          buffer->data[convertedCursor] != ' ' && buffer->data[convertedCursor] != '#' &&
          buffer->data[convertedCursor] != '!' && buffer->data[convertedCursor] != '@' &&
          buffer->data[convertedCursor] != '#' && buffer->data[convertedCursor] != '$' &&
          buffer->data[convertedCursor] != '%' && buffer->data[convertedCursor] != '^' &&
          buffer->data[convertedCursor] != '&' && buffer->data[convertedCursor] != '*' &&
          buffer->data[convertedCursor] != '(' && buffer->data[convertedCursor] != '}' &&
          buffer->data[convertedCursor] != '[' && buffer->data[convertedCursor] != ']' &&
          buffer->data[convertedCursor] != '<' && buffer->data[convertedCursor] != '>' &&
          buffer->data[convertedCursor] != '.' && buffer->data[convertedCursor] != ',' &&
          buffer->data[convertedCursor] != '/' && buffer->data[convertedCursor] != '?' &&
          buffer->data[convertedCursor] != '\\' &&
          !(buffer->data[convertedCursor] >= 'A' && buffer->data[convertedCursor] <= 'Z') &&
          buffer->data[convertedCursor] != '\0'){
        gapDecreaseCursor(buffer);
        convertedCursor = UserToGap(buffer->gap, buffer->cursor);
        if(convertedCursor < 0 || convertedCursor > buffer->size - 1){
            break;
        }
    }
    buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
}

i32 gapGetSelectionSize(GapBuffer* buffer){
    return (buffer->selection.end - buffer->selection.start);
}

void gapSeekCursorINewlinesIfPossible(GapBuffer* buffer, i32 amount){
    if(amount > 0){
        while(--amount){
            gapSeekCursorToNewline(buffer);
        }
    } else if(amount < 0){
        while(++amount){
            gapSeekCursorToPreviousNewline(buffer);
        }
    }
}

i32 gapRemoveCharactersInRange(GapBuffer* buffer, i32 start, i32 end){
    buffer->dirty = true;
    
    // start = UserToGap(buffer->gap, start);
    i32 convertedEnd = end;

    if(start < 0){
        start = 0;
    }

    i32 distance = end - start;

    gapMoveGap(buffer, convertedEnd);
    gapExtendGap(buffer, distance);

    return (end - start);
}

i32 gapInsertNullTerminatedStringAt(GapBuffer* buffer, char* string, i32 position){
    buffer->dirty = true;

    i32 clone = position;
    while(*string){
        gapInsertCharacterAt(buffer, *string, position++);
        ++string;
    }
    return (position - clone);
}

i32 gapInsertStringAt(GapBuffer* buffer, String string, i32 position){
    return gapInsertNullTerminatedStringAt(buffer, (char*)string.data, position);
}


GapBuffer gapCreateEmpty(){
    GapBuffer result = {};

    result.dirty = true;
    result.filename = NULL;
    result.data = new u8[GAP_DEFAULT_SIZE];
    memset(result.data, 0, GAP_DEFAULT_SIZE);
    result.size = GAP_DEFAULT_SIZE;
    result.cursor = 0;
    result.gap.start = 0;
    result.gap.end = GAP_DEFAULT_SIZE;

    return result;
}

GapBuffer gapReadFile(const char* filename){
    GapBuffer result = {};
    
    result.dirty = false;

    FILE* file = fopen(filename, "rb");
    
    if(!file){
        result.data = NULL;
        return result;
    }
 
    result.filename = (char*)filename;

    fseek(file, 0, SEEK_END);
    result.size = ftell(file);
    fseek(file, 0, SEEK_SET);

    result.size += GAP_DEFAULT_SIZE;
    result.data = new u8[result.size];

    memset(result.data, 0, result.size);

    result.cursor = 0;
    result.gap.start = 0;
    result.gap.end = GAP_DEFAULT_SIZE;

    u32 totalReadbytes = 0;
    u32 readBytes = 0;

    fread(result.data + GAP_DEFAULT_SIZE, sizeof(u8), result.size - GAP_DEFAULT_SIZE, file);

    fclose(file);
    return result;
}

void gapWriteFile(GapBuffer* buffer, const char* filename){
    FILE* file = fopen(filename, "wb");
    
    buffer->filename = (char*)filename;
    buffer->dirty = false;

    fwrite(buffer->data, sizeof(u8), buffer->gap.start, file);
    fwrite(buffer->data + buffer->gap.end, sizeof(u8), buffer->size - buffer->gap.end, file);
    fclose(file);
}

void gapWriteFile(GapBuffer* buffer){
    gapWriteFile(buffer, buffer->filename);
}

char* gapToString(GapBuffer* buffer){
    char* result = new char[buffer->size + 1];
    u32 offset = 0;
    for(int i = 0; i < buffer->size; ++i){
        if(i >= buffer->gap.start && i < buffer->gap.end){
            continue;
        } else if (!buffer->data[i]){
            continue;
        }

        result[offset++] = buffer->data[i];
    }

    result[offset] = 0;
    return result;
}

void gapSeekCursorToMatch(GapBuffer* buffer, const char* match){
    // implement boyer moore in the future probably
    for(int i = buffer->cursor + 1; i < buffer->size - 1; ++i){
        bool matching = true;
        i32 convertedCoordinate = UserToGap(buffer->gap, i);
        for(int ii = 0; ii < strlen(match); ++ii){
            if(convertedCoordinate + ii > buffer->size - 1){
                break;
            }
            if(buffer->data[convertedCoordinate + ii] != match[ii]){
                matching = false;
                break;
            }
        }
        if(matching){
            buffer->cursor = i;
            break;
        }
    }
}

void gapClean(GapBuffer* buffer){
    if(buffer->data){
        delete[] buffer->data;
    }

    buffer->data = NULL;
    buffer->size = 0;
    buffer->gap.start = buffer->gap.end = 0;
    buffer->dirty = false;
}









