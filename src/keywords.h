#pragma once

#include "general.h"
#include "math.h"
#include "colors.h"

struct Keyword {
    char* name;
    u8 size;
    v3 color;
};

Keyword KEYWORDS[] = {
    {"#include", 8, KEYWORD_COLOR_SPECIAL},
    {"#if", 3, KEYWORD_COLOR_SPECIAL},
    {"#endif", 6, KEYWORD_COLOR_SPECIAL},
    {"#elif", 5, KEYWORD_COLOR_SPECIAL},
    {"#undef", 6, KEYWORD_COLOR_SPECIAL},
    {"#define", 7, KEYWORD_COLOR_SPECIAL},
    {"#defined", 8, KEYWORD_COLOR_SPECIAL},
    {"#pragma", 7, KEYWORD_COLOR_SPECIAL},

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

    {"u8", 2, KEYWORD_COLOR_SPECIAL},
    {"u16", 3, KEYWORD_COLOR_SPECIAL},
    {"u32", 3, KEYWORD_COLOR_SPECIAL},
    {"u64", 3, KEYWORD_COLOR_SPECIAL},

    {"i8", 2, KEYWORD_COLOR_SPECIAL},
    {"i16", 3, KEYWORD_COLOR_SPECIAL},
    {"i32", 3, KEYWORD_COLOR_SPECIAL},
    {"i64", 3, KEYWORD_COLOR_SPECIAL},

    {"r32", 3, KEYWORD_COLOR_SPECIAL},
    {"r64", 3, KEYWORD_COLOR_SPECIAL}
};

void keywordPeek(u8* data, v3* color, u32* size){
    // NOTE(Sarmis) yes, this has no bounds check
    // this could end up in a segmentation fault
    // good for me I am only reading and not also 
    // writing :D
    u8* neighbourLeft = data - 1;
    //

    if(!(*neighbourLeft >= 'a' && *neighbourLeft <= 'z') &&
       !(*neighbourLeft >= 'A' && *neighbourLeft <= 'Z')){

        for(int i = 0; i < 37; ++i){
            Keyword keyword = KEYWORDS[i];
            u8* clone = data;
            u8* name = (u8*)keyword.name;
            bool match = true;

            while(*clone != ' ' && *clone != '\t' && *clone != '\n' &&
                *clone != ')' && *clone != '(' &&
                *clone != '{' && *clone != '}' &&
                *clone != '!' &&
                *clone != '@' && *clone != '\r' &&
                *clone != ':' && *clone != ';'){
                if(!(clone - data - keyword.size)){
                    break;
                }
                if(*clone++ != *name++){
                    match = false;
                    break;
                }
            }
            u32 referenceSize = clone - data;
            if(match && referenceSize == keyword.size){
                u8 *neighbourRight = clone;
                if(!(*neighbourRight >= 'a' && *neighbourRight <= 'z') &&
                   !(*neighbourRight >= 'A' && *neighbourRight <= 'Z')){

                    *color = KEYWORDS[i].color;
                    *size = referenceSize + 1;
                }
            }
        }
    }
}
