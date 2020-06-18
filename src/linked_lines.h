#pragma once

#include "general.h"

struct LinkedLine {
    struct LinkedLine* previous;
    struct LinkedLine* next;
    Buffer<u8> data;
    i32 cursor;
};

LinkedLine* linkedLineCreate(){
    LinkedLine* result = new LinkedLine();

    result->previous = NULL;
    result->next = NULL;

    result->cursor = 0;

    return result;
}

void linkedLineAppend(LinkedLine* currentLine){
    LinkedLine* newLine = linkedLineCreate();

    newLine->next = currentLine->next;
    currentLine->next = newLine;
    newLine->previous = currentLine;
}

#if 0
void linkedLineAppendBackward(LinkedLine* currentLine){
    LinkedLine* newLine = linkedLineCreate();
    if(currentLine->previous){
        currentLine->previous->next = newLine;
    }

    newLine->previous = currentLine->previous;
    currentLine->previous = newLine;
    newLine->next = currentLine;
}
#endif

void linkedLineClean(LinkedLine* currentLine){
    bufferClean<u8>(&currentLine->data);
    delete currentLine;
}

void linkedRemoveLine(LinkedLine*& currentLine){
    if(currentLine->previous){
        currentLine->previous->next = currentLine->next;
    }

    if(currentLine->next){
        currentLine->next->previous = currentLine->previous;
    }

    linkedLineClean(currentLine);
    currentLine = NULL;
}

void linkedLineSeekCursorControlLeft(LinkedLine* currentLine){
    --currentLine->cursor;
    while(currentLine->data.array[currentLine->cursor] != '\n' &&
          currentLine->data.array[currentLine->cursor] != '\t' &&
          currentLine->data.array[currentLine->cursor] != ' ' && 
          !(currentLine->data.array[currentLine->cursor] >= 'A' && currentLine->data.array[currentLine->cursor] <= 'Z')
          ){

          --currentLine->cursor;
          if(currentLine->cursor < 0){
              currentLine->cursor = 0;
              break;
          }
    }
}

void linkedLineSeekCursorControlRight(LinkedLine* currentLine){
    ++currentLine->cursor;
    while(currentLine->data.array[currentLine->cursor] != '\n' &&
          currentLine->data.array[currentLine->cursor] != '\t' &&
          currentLine->data.array[currentLine->cursor] != ' ' && 
          !(currentLine->data.array[currentLine->cursor] >= 'A' && currentLine->data.array[currentLine->cursor] <= 'Z')
          ){

          ++currentLine->cursor;
          if(currentLine->cursor > currentLine->data.currentAmount){
              currentLine->cursor = currentLine->data.currentAmount;
              break;
          }
    }
}

i32 linkedLineGetLineYOffset(LinkedLine* firstLine, LinkedLine* currentLine){
    int result = 0;
    while(firstLine && firstLine != currentLine){
        ++result;
        firstLine = firstLine->next;
    }
    return result;
}


i32 linkedLineGetWidthFromRange(LinkedLine* currentLine, i32 start, i32 end){
    int result = 0;
    for(int i = start; i <= end; ++i){
        switch (currentLine->data.array[i]) {
            case '\t':{
                    result += 3;
                }
                break;
            
            default: {
                    ++result;
                }
                break;
        }
    }
    return result;
}