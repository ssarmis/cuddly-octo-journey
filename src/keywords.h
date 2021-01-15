#pragma once

#include "general.h"
#include "math.h"
#include "colors.h"

struct Keyword {
    char* name;
    u8 size;
    v4 color;
};

static Keyword KEYWORDS[] = {
    {"#include", 8, KEYWORD_COLOR_SPECIAL},
    {"#ifdef", 6, KEYWORD_COLOR_SPECIAL},
    {"#endif", 6, KEYWORD_COLOR_SPECIAL},
    {"#elif", 5, KEYWORD_COLOR_SPECIAL},
    {"#else", 5, KEYWORD_COLOR_SPECIAL},
    {"#undef", 6, KEYWORD_COLOR_SPECIAL},
    {"#defined", 8, KEYWORD_COLOR_SPECIAL},
    {"#define", 7, KEYWORD_COLOR_SPECIAL},
    {"#pragma", 7, KEYWORD_COLOR_SPECIAL},
    {"#if", 3, KEYWORD_COLOR_SPECIAL},
    {"bool", 4, KEYWORD_COLOR_TYPE},
    {"unsigned", 8, KEYWORD_COLOR_TYPE},
    {"char", 4, KEYWORD_COLOR_TYPE},
    {"short", 5, KEYWORD_COLOR_TYPE},
    {"int", 3, KEYWORD_COLOR_TYPE},
    {"float", 5, KEYWORD_COLOR_TYPE},
    {"double", 6, KEYWORD_COLOR_TYPE},
    {"void", 4, KEYWORD_COLOR_TYPE},
    {"return", 6, KEYWORD_COLOR_SPECIAL},
    {"for", 3, KEYWORD_COLOR_SPECIAL},
    {"while", 5, KEYWORD_COLOR_SPECIAL},
    {"if", 2, KEYWORD_COLOR_SPECIAL},
    {"switch", 6, KEYWORD_COLOR_SPECIAL},
    {"case", 4, KEYWORD_COLOR_SPECIAL},
    {"else", 4, KEYWORD_COLOR_SPECIAL},
    {"default", 7, KEYWORD_COLOR_SPECIAL},
    {"break", 5, KEYWORD_COLOR_SPECIAL},
    {"const", 5, KEYWORD_COLOR_SPECIAL},
    {"typedef", 7, KEYWORD_COLOR_SPECIAL},
    {"struct", 6, KEYWORD_COLOR_SPECIAL},
    {"static", 6, KEYWORD_COLOR_SPECIAL}
    // {"u8", 2, KEYWORD_COLOR_SPECIAL},
    // {"u16", 3, KEYWORD_COLOR_SPECIAL},
    // {"u32", 3, KEYWORD_COLOR_SPECIAL},
    // {"u64", 3, KEYWORD_COLOR_SPECIAL},
    // {"i8", 2, KEYWORD_COLOR_SPECIAL},
    // {"i16", 3, KEYWORD_COLOR_SPECIAL},
    // {"i32", 3, KEYWORD_COLOR_SPECIAL},
    // {"i64", 3, KEYWORD_COLOR_SPECIAL},
    // {"r32", 3, KEYWORD_COLOR_SPECIAL},
    // {"r64", 3, KEYWORD_COLOR_SPECIAL}
};

static bool shouldStop(char c){
    return (c != ' ' && c != '\t' && 
            c != ')' && c != '('  &&
            c != '{' && c != '}'  &&
            c != '!' && c != '\n' &&
            c != '@' && c != '\r' &&
            c != ':' && c != ';');
}

static void keywordPeek(GapBuffer* buffer, u32 index, v4* color, u32* size){
    for(int i = 0; i < 31; ++i){
        i32 startingIndex = index;
        Keyword keyword = KEYWORDS[i];
        u8* keywordName = (u8*)keyword.name;
        bool match = true;

        while(*keywordName && shouldStop(gapCharacterAtIndex(buffer, startingIndex))){
            if(gapCharacterAtIndex(buffer, startingIndex) != *keywordName++){
                match = false;
                break;
            }
            startingIndex++;
        }

        if(match && startingIndex != index && 
           (startingIndex - index) == keyword.size){
            *color = keyword.color;
            *size = keyword.size;
            break;
        }
    }
#if 0
    u32 allowedDistance = buffer->size - index;
    for(int i = 0; i < 38; ++i){
        Keyword keyword = KEYWORDS[i];
        u8* clone = &buffer->data[index];
        u8* name = (u8*)keyword.name;
        bool match = true;

        while(shouldStop(*clone)){
            if(!(clone - &buffer->data[index] - keyword.size)){
                break;
            }
            if(*clone++ != *name++){
                match = false;
                break;
            }
            if(clone - &buffer->data[index] > allowedDistance){
                break;
            }
        }
        u32 referenceSize = clone - &buffer->data[index];
        if(match && referenceSize == keyword.size){
            u8 *neighbourRight = clone;
            if(!(*neighbourRight >= 'a' && *neighbourRight <= 'z') &&
               !(*neighbourRight >= 'A' && *neighbourRight <= 'Z')){

                *color = KEYWORDS[i].color;
                *size = referenceSize + 1;
            }
        }
    }
#endif
}
  