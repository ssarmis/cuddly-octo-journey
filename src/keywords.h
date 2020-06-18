#pragma once

#include "general.h"
#include "math.h"

struct Keyword {
    char* name;
    u8 size;
    v3 color;
};

v3 KEYWORD_COLOR_TYPE = {0, 0.5, 1};

v3 KEYWORD_COLOR_SPECIAL = KEYWORD_COLOR_TYPE;



Keyword KEYWORDS[] = {
    {"#include", 8, KEYWORD_COLOR_SPECIAL},
    {"#if", 3, KEYWORD_COLOR_SPECIAL},
    {"#endif", 6, KEYWORD_COLOR_SPECIAL},
    {"#elif", 5, KEYWORD_COLOR_SPECIAL},
    {"#undef", 6, KEYWORD_COLOR_SPECIAL},
    {"#define", 7, KEYWORD_COLOR_SPECIAL},
    {"#defined", 8, KEYWORD_COLOR_SPECIAL},

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
    {"case", 4, KEYWORD_COLOR_SPECIAL}
};

void keywordPeek(u8* data, v3* color, u32* size){
    for(int i = 0; i < 20; ++i){
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
            *color = KEYWORDS[i].color;
            *size = referenceSize + 1;
        }
    }
}