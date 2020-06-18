#pragma once

#include "general.h"

#define GAP_DEFAULT_SIZE 4

#define UserToGap(gap, x) (((x) < gap.start) ? (x) :( (gap.end - gap.start) + (x)))

struct Gap {
    u32 start;
    u32 end;
};

struct Selection {
    i32 start;
    i32 end;
};

struct GapBuffer {
    FILE* file;
    union{
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

void gapInitializeGap(GapBuffer* buffer, u32 position, u32 size){
    buffer->gap.start = position;
    buffer->gap.end = buffer->gap.start + size;
    // TODO(Sarmis) add assertions for when outside buffer
}

void gapGrowGap(GapBuffer* buffer, u32 size){
    u8* clone = buffer->data;
    
    buffer->data = new u8[buffer->size + size];
    memset(buffer->data, 0, buffer->size + size);

    for(int i = 0; i < buffer->gap.start; ++i){
        buffer->data[i] = clone[i];
    }

    for(int i = buffer->gap.end; i < buffer->size; ++i){
        buffer->data[i + size] = clone[i];
    }

    buffer->gap.end = buffer->gap.start + size;

    buffer->size += size;

    delete[] clone;
}

void gapMoveCursor(GapBuffer* buffer, u32 position){
    if(position < 0 && position <= buffer->size){
        return;
    }

    if(position > buffer->gap.start && position <= buffer->gap.end){
        position = buffer->gap.end;
    }

    buffer->cursor = position;
}

void gapMoveGap(GapBuffer* buffer, u32 position){
    if(position == buffer->gap.start){
        return;
    }


    if(position < buffer->gap.start){
        u32 shiftAmount = buffer->gap.start - position;
        while(shiftAmount--){
            buffer->data[buffer->gap.end-- - 1] = buffer->data[buffer->gap.start-- - 1];
        }
    } else if(position > buffer->gap.end){
        u32 shiftAmount = position - buffer->gap.end;
        while(shiftAmount--){
            buffer->data[buffer->gap.start++] = buffer->data[buffer->gap.end++];
        }
    }
}

void gapReplaceCharacterAt(GapBuffer* buffer, char character, u32 position){
    if(position < 0 && position <= buffer->size){
        return;
    }
    buffer->data[position] = character;
}

void gapShrinkGap(GapBuffer* buffer){
    if(buffer->gap.start != buffer->gap.end){
        ++buffer->gap.start;
    }
}

void gapExtendGap(GapBuffer* buffer){
    --buffer->gap.start;
}

void gapExtendGapBackwards(GapBuffer* buffer){
    ++buffer->gap.end;
}

void gapInsertCharacterAt(GapBuffer* buffer, char character, u32 position){
    gapMoveGap(buffer, UserToGap(buffer->gap, position));

    gapReplaceCharacterAt(buffer, character, buffer->gap.start);
    gapShrinkGap(buffer); // basically ++gap.start

    if(!gapGetGapSize(buffer)){
        gapGrowGap(buffer, GAP_DEFAULT_SIZE);
    }
}

void gapRemoveCharacterNearAt(GapBuffer* buffer, u32 position){
    gapMoveGap(buffer, UserToGap(buffer->gap, position));
    gapExtendGapBackwards(buffer); // basically ++gap.end
}

void gapRemoveCharacterAt(GapBuffer* buffer, u32 position){
    gapMoveGap(buffer, UserToGap(buffer->gap, position));
    gapExtendGap(buffer); // basically --gap.start
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
}

i32 gapGetDistanceToNewline(GapBuffer* buffer){
    i32 clone = buffer->cursor;
    i32 convertedCursor = UserToGap(buffer->gap, buffer->cursor);

    // TODO(Sarmis) since the end of the buffer might me bagic
    // just let this here for now..
    while(buffer->data[convertedCursor] != '\n' &&
          buffer->data[convertedCursor] != '\0'){
        ++clone;
        if(clone == buffer->size){
            break;
        }
        convertedCursor = UserToGap(buffer->gap, buffer->cursor);
    }
    return (clone - buffer->cursor);
}

i32 gapGetDistanceFromPreviousNewline(GapBuffer* buffer){
    i32 clone = buffer->cursor;
    i32 convertedCursor = UserToGap(buffer->gap, buffer->cursor);

    while(buffer->data[convertedCursor] != '\n'){
        --clone;
        if(clone <= 0){
            break;
        }
        convertedCursor = UserToGap(buffer->gap, clone);
    }
    return (buffer->cursor - clone);
}

void gapSeekCursorToPreviousTabOrNewline(GapBuffer* buffer){
    --buffer->cursor;
    i32 convertedCursor = UserToGap(buffer->gap, buffer->cursor);

    while(buffer->data[convertedCursor] != '\t' &&
          buffer->data[convertedCursor] != '\n'){
        --buffer->cursor;
        if(buffer->cursor <= 0){
            break;
        }
        convertedCursor = UserToGap(buffer->gap, buffer->cursor);
    }
    buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
}

void gapSeekCursorToPreviousNewline(GapBuffer* buffer){
    --buffer->cursor;
    i32 convertedCursor = UserToGap(buffer->gap, buffer->cursor);

    while(buffer->data[convertedCursor] != '\n'){
        --buffer->cursor;
        if(buffer->cursor <= 0){
            break;
        }
        convertedCursor = UserToGap(buffer->gap, buffer->cursor);
    }
    buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
}

i32 gapGetAmontOfTabsBeforeCursor(GapBuffer* buffer){
    i32 result = 0;
    i32 clone = buffer->cursor;
    --clone;
    i32 convertedCursor = UserToGap(buffer->gap, clone);

    while(buffer->data[convertedCursor] != '\n'){
        if(buffer->data[convertedCursor] == '\t'){
            ++result;
        }

        --clone;
        if(clone <= 0){
            break;
        }
        convertedCursor = UserToGap(buffer->gap, clone);
    }

    return result;
}

void gapSeekCursorToNewline(GapBuffer* buffer){
    ++buffer->cursor;
    i32 convertedCursor = UserToGap(buffer->gap, buffer->cursor);

    // TODO(Sarmis) since the end of the buffer might me bagic
    // just let this here for now..
    while(buffer->data[convertedCursor] != '\n' &&
          buffer->data[convertedCursor] != '\0'){
        ++buffer->cursor;
        if(buffer->cursor == buffer->size - 1){
            break;
        }
        convertedCursor = UserToGap(buffer->gap, buffer->cursor);
    }
    buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
}

i32 gapSeekGetConsecutiveSpaces(GapBuffer* buffer){
    i32 clone = buffer->cursor;

    ++clone;
    i32 convertedCursor = UserToGap(buffer->gap, clone);

    // TODO(Sarmis) since the end of the buffer might me bagic
    // just let this here for now..
    while((buffer->data[convertedCursor] == ' '  ||
          buffer->data[convertedCursor] == '\t') &&
          buffer->data[convertedCursor] != '\0'){
        ++clone;
        if(clone == buffer->size){
            break;
        }
        convertedCursor = UserToGap(buffer->gap, clone);
    }
    return (clone - buffer->cursor);
}

void gapSeekCursorToCapitalOrSpace(GapBuffer* buffer){
    i32 possibleSkip = gapSeekGetConsecutiveSpaces(buffer);
    printf("%d\n", possibleSkip);
    if(possibleSkip > 1){
        buffer->cursor += possibleSkip;
        return;
    }
    ++buffer->cursor;
    i32 convertedCursor = UserToGap(buffer->gap, buffer->cursor);

    // TODO(Sarmis) since the end of the buffer might me bagic
    // just let this here for now..
    while(buffer->data[convertedCursor] != '\n' &&
          buffer->data[convertedCursor] != ' ' &&
          buffer->data[convertedCursor] != '!' &&
          buffer->data[convertedCursor] != '@' &&
          buffer->data[convertedCursor] != '#' &&
          buffer->data[convertedCursor] != '$' &&
          buffer->data[convertedCursor] != '%' &&
          buffer->data[convertedCursor] != '^' &&
          buffer->data[convertedCursor] != '&' &&
          buffer->data[convertedCursor] != '*' &&
          buffer->data[convertedCursor] != '(' &&
          buffer->data[convertedCursor] != '}' &&
          buffer->data[convertedCursor] != '[' &&
          buffer->data[convertedCursor] != ']' &&
          buffer->data[convertedCursor] != '<' &&
          buffer->data[convertedCursor] != '>' &&
          buffer->data[convertedCursor] != '.' &&
          buffer->data[convertedCursor] != ',' &&
          buffer->data[convertedCursor] != '/' &&
          buffer->data[convertedCursor] != '?' &&
          buffer->data[convertedCursor] != '\\' &&
          !(buffer->data[convertedCursor] >= 'A' && buffer->data[convertedCursor] <= 'Z') &&
          buffer->data[convertedCursor] != '\0'){
        ++buffer->cursor;
        if(buffer->cursor == buffer->size){
            break;
        }
        convertedCursor = UserToGap(buffer->gap, buffer->cursor);
    }
    buffer->cursor = clamp(buffer->cursor, 0, buffer->size - 1);
}

void gapSeekCursorToPreviousCapitalOrSpace(GapBuffer* buffer){
    --buffer->cursor;
    i32 convertedCursor = UserToGap(buffer->gap, buffer->cursor);
    // Ok for real I will make a function to see if its a symbol instead
    // of untolling it
    while(buffer->data[convertedCursor] != '\n' &&
          buffer->data[convertedCursor] != ' ' &&
          buffer->data[convertedCursor] != '!' &&
          buffer->data[convertedCursor] != '@' &&
          buffer->data[convertedCursor] != '#' &&
          buffer->data[convertedCursor] != '$' &&
          buffer->data[convertedCursor] != '%' &&
          buffer->data[convertedCursor] != '^' &&
          buffer->data[convertedCursor] != '&' &&
          buffer->data[convertedCursor] != '*' &&
          buffer->data[convertedCursor] != '(' &&
          buffer->data[convertedCursor] != '}' &&
          buffer->data[convertedCursor] != '[' &&
          buffer->data[convertedCursor] != ']' &&
          buffer->data[convertedCursor] != '<' &&
          buffer->data[convertedCursor] != '>' &&
          buffer->data[convertedCursor] != '.' &&
          buffer->data[convertedCursor] != ',' &&
          buffer->data[convertedCursor] != '/' &&
          buffer->data[convertedCursor] != '?' &&
          buffer->data[convertedCursor] != '\\' &&
          !(buffer->data[convertedCursor] >= 'A' && buffer->data[convertedCursor] <= 'Z') &&
          buffer->data[convertedCursor] != '\0'){
        --buffer->cursor;
        if(buffer->cursor == 0){
            break;
        }
        convertedCursor = UserToGap(buffer->gap, buffer->cursor);
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

i32 gapInsertNullTerminatedStringAt(GapBuffer* buffer, char* string, i32 position){
    i32 clone = position;
    while(*string){
        gapInsertCharacterAt(buffer, *string, position++);
        ++string;
    }
    return (position - clone);
}

GapBuffer gapReadFile(const char* filename){
    GapBuffer result = {};

    FILE* file = fopen(filename, "rb");
    
    ASSERT(file);

    result.file = file;

    fseek(file, 0, SEEK_END);
    result.size = ftell(file);
    fseek(file, 0, SEEK_SET);

    result.data = new u8[result.size + GAP_DEFAULT_SIZE];

    memset(result.data, 0, result.size + GAP_DEFAULT_SIZE);

    result.cursor = 0;
    result.gap.start = 0;
    result.gap.end = GAP_DEFAULT_SIZE;

    while(fread(result.data + GAP_DEFAULT_SIZE, sizeof(u8), result.size, file)){
    }

    return result;
}

