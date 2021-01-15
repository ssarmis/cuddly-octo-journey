#pragma once

#include <stdio.h>
#include "general.h"

#define GAP_DEFAULT_SIZE 4
// 012345
// ABCDEF

// 01234567
// AB__CDEF
#define gapUserToGap(gap, x) (((x) < gap.start) ? (x) :( (gap.end - gap.start) + (x)))
#define gapGapToUser(gap, x) ((x > gap.start) ? (x - (gap.end - gap.start)) : (x))
#define gapGetGapSize(buffer) ((buffer)->gap.end - (buffer)->gap.start)
#define gapGetAbstractSize(buffer) ((buffer)->size - gapGetGapSize(buffer))
#define gapCharacterAtIndex(buffer, index) ((buffer)->data[gapUserToGap((buffer)->gap, index)])
#define gapGetCursorCharacter(buffer) gapCharacterAtIndex(buffer, (buffer)->cursor)

#define gapIncreaseCursor(buffer) {\
    ++(buffer)->cursor;\
    (buffer)->cursor = cuddle_clamp((buffer)->cursor, 0, gapGetAbstractSize(buffer) - 1);\
}

#define gapDecreaseCursor(buffer) {\
    --(buffer)->cursor;\
    (buffer)->cursor = cuddle_clamp((buffer)->cursor, 0, gapGetAbstractSize(buffer) - 1);\
}

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

// #ifdef DEBUG_BUILD
static void gapPrintGap(GapBuffer* buffer){
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

static void gapPrintGap(char* sink, GapBuffer* buffer){
    for(int i = 0; i < buffer->size; ++i){
        if(i >= buffer->gap.start && i < buffer->gap.end){
            sprintf(sink++, "_");
            continue;
        }
        if(!buffer->data[i]){
            sprintf(sink++, "@");
            continue;
        }
        if(buffer->data[i] == '\n'){
            sprintf(sink, "\\n");
            sink += 2;
        } else {
            sprintf(sink++, "%c", buffer->data[i]);
        }
    }

    sprintf(sink, "\ncursor at = %d\n", buffer->cursor);
}
// #endif

static void gapInitializeGap(GapBuffer* buffer, i32 position, i32 size){
    buffer->gap.start = position;
    buffer->gap.end = buffer->gap.start + size;
    // TODO(Sarmis) add assertions for when outside buffer
}

static void gapGrowGap(GapBuffer* buffer, i32 size){
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

static void gapMoveCursor(GapBuffer* buffer, i32 position){
    if(position < 0 || position > buffer->size - 1){
        return;
    }

    buffer->cursor = position;
}

static void gapMoveGap(GapBuffer* buffer, i32 position){
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

static void gapReplaceCharacterAt(GapBuffer* buffer, char character, u32 position){
    if(position < 0 || position > buffer->size - 1){
        return;
    }
    buffer->data[position] = character;
}

static void gapShrinkGap(GapBuffer* buffer){
    if(buffer->gap.start != buffer->gap.end){
        ++buffer->gap.start;
    }
}

static void gapExtendGap(GapBuffer* buffer, i32 amount){
    buffer->gap.start -= amount;
    buffer->gap.start = cuddle_clamp(buffer->gap.start, 0, buffer->size - 1);
}

static void gapExtendGap(GapBuffer* buffer){
    gapExtendGap(buffer, 1);
}

static void gapExtendGapBackwards(GapBuffer* buffer){
    ++buffer->gap.end;
    buffer->gap.end = cuddle_clamp(buffer->gap.end, 0, buffer->size - 1);
}

static void gapInsertCharacterAt(GapBuffer* buffer, char character, i32 position){
    buffer->dirty = true;
    position = gapUserToGap(buffer->gap, position);
    gapMoveGap(buffer, position);

    gapReplaceCharacterAt(buffer, character, buffer->gap.start);
    gapShrinkGap(buffer); // basically ++gap.start

    if(!gapGetGapSize(buffer)){
        gapGrowGap(buffer, GAP_DEFAULT_SIZE);
    }
}

static void gapRemoveCharacterNearAt(GapBuffer* buffer, i32 position){
    buffer->dirty = true;
    if(gapGetGapSize(buffer) + position > buffer->size){
        return;
    }
    gapMoveGap(buffer, gapUserToGap(buffer->gap, position));
    gapExtendGapBackwards(buffer); // basically ++gap.end
    buffer->data[buffer->gap.end - 1] = 0;
}

static void gapRemoveCharacterAt(GapBuffer* buffer, i32 position){
    buffer->dirty = true;
    gapMoveGap(buffer, gapUserToGap(buffer->gap, position));
    gapExtendGap(buffer); // basically --gap.start
    buffer->data[buffer->gap.start] = 0;
}

static bool gapPointInGap(GapBuffer* buffer, i32 point){
    return (point >= buffer->gap.start && point < buffer->gap.end);
}

static bool gapCursorInGap(GapBuffer* buffer){
    return (buffer->cursor >= buffer->gap.start && buffer->cursor < buffer->gap.end);
}

// AA____B

static void gapSeekCursor(GapBuffer* buffer, i32 distance){
    buffer->cursor += distance;
    buffer->cursor = cuddle_clamp(buffer->cursor, 0, gapGetAbstractSize(buffer) - 1);
}

static i32 gapGetDistanceToNewline(GapBuffer* buffer){
    i32 clone = buffer->cursor;

    // TODO(Sarmis) since the end of the buffer might me bagic
    // just let this here for now..
    while(gapCharacterAtIndex(buffer, clone) != '\n' &&
          gapCharacterAtIndex(buffer, clone) != '\r'){

        if(clone >= gapGetAbstractSize(buffer) - 1){
            break;
        }
        ++clone;
    }
    clone = cuddle_clamp(clone, 0, gapGetAbstractSize(buffer) - 1);
    return (clone - buffer->cursor);
}

static i32 gapGetDistanceFromPreviousNewline(GapBuffer* buffer){
    i32 clone = buffer->cursor;
    
    while(gapCharacterAtIndex(buffer, clone) != '\n' &&
          gapCharacterAtIndex(buffer, clone) != '\r'){
        --clone;
        if(clone <= 0){
            break;
        }
    }

    return (buffer->cursor - clone);
}

static void gapSeekCursorToPreviousTabOrNewline(GapBuffer* buffer){
    gapDecreaseCursor(buffer);
   
    while(gapGetCursorCharacter(buffer) != '\t' &&
          gapGetCursorCharacter(buffer) != '\n'){
        gapDecreaseCursor(buffer);

        if(buffer->cursor == 0){
            break;
        }
    }
}

static void gapSeekCursorToPreviousNewline(GapBuffer* buffer){
    if(gapGetCursorCharacter(buffer) == '\n' || 
       gapGetCursorCharacter(buffer) == '\r' ||
       gapGetCursorCharacter(buffer) == '\t' ){
        gapDecreaseCursor(buffer);
    }

    while(gapGetCursorCharacter(buffer) != '\n' &&
          gapGetCursorCharacter(buffer) != '\r'){
        gapDecreaseCursor(buffer);
        if(buffer->cursor == 0){
            break;
        }
    }
}

static void gapSeekCursorToNewline(GapBuffer* buffer){
    if(gapGetCursorCharacter(buffer) == '\n' || gapGetCursorCharacter(buffer) == '\r'){
        gapIncreaseCursor(buffer);
    }

    while(gapGetCursorCharacter(buffer) != '\n' &&
          gapGetCursorCharacter(buffer) != '\r'){
        gapIncreaseCursor(buffer);
        if(buffer->cursor == gapGetAbstractSize(buffer) - 1){
            return;
        }
    }
}

static i32 gapGetAmontOfTabsBeforeCursor(GapBuffer* buffer){
    i32 result = 0;

    i32 clone = buffer->cursor;

    while(buffer->data[clone] != '\n'){
        if(buffer->data[clone] == '\t'){
            ++result;
        }

        --clone;
        if(clone <= 0){
            break;
        }
    }

    return result;
}

static void gapSeekCursorToLine(GapBuffer* buffer, i32 line){
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
    buffer->cursor = cuddle_clamp(buffer->cursor, 0, buffer->size - 1);
}

static i32 gapGetConsecutiveSpaces(GapBuffer* buffer){
    i32 clone = buffer->cursor;

    // TODO(Sarmis) since the end of the buffer might me bagic
    // just let this here for now..
    while((gapCharacterAtIndex(buffer, clone) == ' '  ||
          gapCharacterAtIndex(buffer, clone) == '\t')){
        ++clone;
        
        if(clone >= gapGetAbstractSize(buffer) - 1){
            break;
        }
    }

    clone = cuddle_clamp(clone, 0, gapGetAbstractSize(buffer) - 1);
    return (clone - buffer->cursor);
}

static i32 gapGetPreviousConsecutiveSpaces(GapBuffer* buffer){
    i32 clone = buffer->cursor;

    --clone;
    if(clone < 0){
        return 0;    
    }
    // TODO(Sarmis) since the end of the buffer might me bagic
    // just let this here for now..
    
    while((gapCharacterAtIndex(buffer, clone) == ' '  ||
          gapCharacterAtIndex(buffer, clone) == '\t')){

        --clone;
        if(clone <= 0){
            break;
        }
    }

    clone = cuddle_clamp(clone, 0, gapGetAbstractSize(buffer) - 1);
    return (buffer->cursor - clone);
}

static void gapSeekCursorToSymbolOrSpace(GapBuffer* buffer){
    i32 possibleSkip = gapGetConsecutiveSpaces(buffer);
    if(possibleSkip > 1){
        buffer->cursor += possibleSkip;
        return;
    }

    gapIncreaseCursor(buffer);

    // TODO(Sarmis) since the end of the buffer might me bagic
    // just let this here for now..
    
    while(gapGetCursorCharacter(buffer) != '\n' && gapGetCursorCharacter(buffer) != '"' &&
          gapGetCursorCharacter(buffer) != ' ' && gapGetCursorCharacter(buffer) != '!' &&
          gapGetCursorCharacter(buffer) != '@' && gapGetCursorCharacter(buffer) != '#' &&
          gapGetCursorCharacter(buffer) != '-' && gapGetCursorCharacter(buffer) != '+' &&
          gapGetCursorCharacter(buffer) != '$' && gapGetCursorCharacter(buffer) != '%' &&
          gapGetCursorCharacter(buffer) != '=' &&
          gapGetCursorCharacter(buffer) != '^' && gapGetCursorCharacter(buffer) != '&' &&
          gapGetCursorCharacter(buffer) != '*' && gapGetCursorCharacter(buffer) != '(' &&
          gapGetCursorCharacter(buffer) != '}' && gapGetCursorCharacter(buffer) != '[' &&
          gapGetCursorCharacter(buffer) != ']' && gapGetCursorCharacter(buffer) != '<' &&
          gapGetCursorCharacter(buffer) != '>' && gapGetCursorCharacter(buffer) != '.' &&
          gapGetCursorCharacter(buffer) != ',' && gapGetCursorCharacter(buffer) != '/' &&
          gapGetCursorCharacter(buffer) != '?' && gapGetCursorCharacter(buffer) != '\\' &&
          gapGetCursorCharacter(buffer) != ')' && gapGetCursorCharacter(buffer) != '\t' &&
          gapGetCursorCharacter(buffer) != ';'){
        
        gapIncreaseCursor(buffer);

        if(buffer->cursor >= gapGetAbstractSize(buffer) - 1){
            break;
        }
    }
}

static void gapSeekCursorToPreviousSymbolOrSpace(GapBuffer* buffer){
    i32 possibleSkip = gapGetPreviousConsecutiveSpaces(buffer);
    if(possibleSkip > 1){
        gapSeekCursor(buffer, -possibleSkip + 1);
        return;
    }
    
    gapDecreaseCursor(buffer);
	bool moved = false;
    // Ok for real I will make a function to see if its a symbol instead
    // of untolling it
    while(gapGetCursorCharacter(buffer) != '\n' && gapGetCursorCharacter(buffer) != '"' &&
          gapGetCursorCharacter(buffer) != ' ' && gapGetCursorCharacter(buffer) != '!' &&
          gapGetCursorCharacter(buffer) != '@' && gapGetCursorCharacter(buffer) != '#' &&
          gapGetCursorCharacter(buffer) != '-' && gapGetCursorCharacter(buffer) != '+' &&
          gapGetCursorCharacter(buffer) != '$' && gapGetCursorCharacter(buffer) != '%' &&
          gapGetCursorCharacter(buffer) != '=' &&
          gapGetCursorCharacter(buffer) != '^' && gapGetCursorCharacter(buffer) != '&' &&
          gapGetCursorCharacter(buffer) != '*' && gapGetCursorCharacter(buffer) != '(' &&
          gapGetCursorCharacter(buffer) != '}' && gapGetCursorCharacter(buffer) != '[' &&
          gapGetCursorCharacter(buffer) != ']' && gapGetCursorCharacter(buffer) != '<' &&
          gapGetCursorCharacter(buffer) != '>' && gapGetCursorCharacter(buffer) != '.' &&
          gapGetCursorCharacter(buffer) != ',' && gapGetCursorCharacter(buffer) != '/' &&
          gapGetCursorCharacter(buffer) != '?' && gapGetCursorCharacter(buffer) != '\\' &&
          gapGetCursorCharacter(buffer) != ')' && gapGetCursorCharacter(buffer) != '\t' &&
          gapGetCursorCharacter(buffer) != ';'){

        gapDecreaseCursor(buffer);
 	   moved = true;
        if(buffer->cursor <= 0){
            break;
        }
	}

	if(moved){
		gapIncreaseCursor(buffer);
	}
}

static i32 gapGetSelectionSize(GapBuffer* buffer){
    return (buffer->selection.end - buffer->selection.start);
}

static void gapSeekCursorINewlinesIfPossible(GapBuffer* buffer, i32 amount){
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

static i32 gapRemoveCharactersInRange(GapBuffer* buffer, i32 start, i32 end){
    buffer->dirty = true;
    
    ASSERT(end > start);

    if(start < 0){
        start = 0;
    }

    i32 distance = end - start;

    gapMoveGap(buffer, gapUserToGap(buffer->gap, end));
    gapExtendGap(buffer, distance);

    return (end - start);
}

static i32 gapInsertNullTerminatedStringAt(GapBuffer* buffer, char* string, i32 position){
    buffer->dirty = true;

    i32 clone = position;
    while(*string){
        gapInsertCharacterAt(buffer, *string, position++);
        ++string;
    }
    return (position - clone);
}

static i32 gapInsertStringAt(GapBuffer* buffer, String string, i32 position){
    return gapInsertNullTerminatedStringAt(buffer, (char*)string.data, position);
}

static void shiftMemory(u8* source, u8* destination, u32 amount){
    ASSERT(source && destination);
    while(amount--){
        u8 aux = *destination;
        *destination++ = *source;
        *source++ = aux;
    }
}

static void shiftMemoryNoZeros(u8* source, u8* destination, u32 amount){
    ASSERT(source && destination);
    while(amount){
        if(*source){
            u8 aux = *destination;
            *destination++ = *source;
            *source = aux;
            amount--;
        }
        *source++;
    }
}

static void gapMoveRange(GapBuffer* buffer, i32 start, i32 end, i32 position){
    if(position == start){
        return;
    }

    end = gapUserToGap(buffer->gap, end);
    start = gapUserToGap(buffer->gap, start);
    position = gapUserToGap(buffer->gap, position);
    u32 amount = end - start;

    shiftMemoryNoZeros(&buffer->data[start], &buffer->data[position], amount);
}

static GapBuffer gapCreateEmpty(){
    GapBuffer result = {};

    result.dirty = true;
    result.filename = NULL;
    result.data = new u8[GAP_DEFAULT_SIZE + 1];
    memset(result.data, 0, GAP_DEFAULT_SIZE + 1);
    result.size = GAP_DEFAULT_SIZE + 1;
    result.data[0] = ' ';
    result.cursor = 0;
    result.gap.start = 1;
    result.gap.end = result.gap.start + GAP_DEFAULT_SIZE;

    return result;
}

static GapBuffer gapReadFile(const char* filename){
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

    result.size += GAP_DEFAULT_SIZE + 1;
    result.data = new u8[result.size];

    memset(result.data, 0, result.size);

    result.cursor = 0;
    result.gap.start = 0;
    result.gap.end = GAP_DEFAULT_SIZE;

    u32 totalReadbytes = 0;
    u32 readBytes = 0;

    fread(result.data + GAP_DEFAULT_SIZE, sizeof(u8), result.size - 1 - GAP_DEFAULT_SIZE, file);
    
    result.data[result.size - 1] = ' ';
    
    fclose(file);
    return result;
}

static void gapWriteFile(GapBuffer* buffer, const char* filename){
    FILE* file = fopen(filename, "wb");
    
    buffer->filename = (char*)filename;
    buffer->dirty = false;

    fwrite(buffer->data, sizeof(u8), buffer->gap.start, file);
    fwrite(buffer->data + buffer->gap.end, sizeof(u8), buffer->size - buffer->gap.end, file);
    fclose(file);
}

static void gapWriteFile(GapBuffer* buffer){
    gapWriteFile(buffer, buffer->filename);
}

static char* gapToString(GapBuffer* buffer){
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

    result[offset-1] = 0; // prevent space copying
    return result;
}

static Selection gapSeekIndexToMatch(GapBuffer* buffer, const char* match, i32* index, u32 start = 0){
    Selection result = {};
    // implement boyer moore in the future probably
    for(int i = start; i < gapGetAbstractSize(buffer); ++i){
        bool matching = true;
        result.start = i;
        for(int ii = 0; ii < strlen(match); ++ii){
            if(i + ii > gapGetAbstractSize(buffer) - 1){
                result = {};
                break;
            }
            if(gapCharacterAtIndex(buffer, (i + ii)) != match[ii]){
                matching = false;
                result = {};
                break;
            }
            result.end = i + ii + 1;
        }
        if(matching){
            *index = i;
            break;
        }
    }
    return result;
}

static Selection gapSeekCursorToMatch(GapBuffer* buffer, const char* match, u32 start = 0){
    return gapSeekIndexToMatch(buffer, match, &buffer->cursor, start);
}

static void gapClean(GapBuffer* buffer){
    if(buffer->data){
        delete[] buffer->data;
    }

    buffer->selection = {};
    buffer->data = NULL;
    buffer->size = 0;
    buffer->gap.start = buffer->gap.end = 0;
    buffer->dirty = false;
}

static String gapGetSubString(GapBuffer* buffer, i32 start, i32 end){
    ASSERT(end > start);

    String result = {};

    result.size = end - start;
    result.data = new u8[result.size + 1];
    result.data[result.size] = 0;

    u32 offset = 0;
    for(int i = start; i < end; ++i){
        result.data[offset++] = gapCharacterAtIndex(buffer, i);
    }

    return result;
}







    
