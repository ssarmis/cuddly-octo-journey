#pragma once

#include <string.h>
#include <stdint.h>

#ifdef DEBUG_BUILD
#include <stdio.h>
#include <assert.h>
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float r32;
typedef double r64;

typedef u32 Index;

#include <time.h>

#define KB(x) ((1 << 10) * (x))
#define MB(x) (KB(1024) * (x))

#ifdef DEBUG_BUILD
#define TRACE(...) fprintf(stdout, __VA_ARGS__)
#define ASSERT(x) assert(x)
#else
#define TRACE(...) 
#define ASSERT(x)
#endif

#define BENCHMARK_SCOPE(name, scope) {\
    struct timespec start = {};\
    struct timespec end = {};\
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);\
    scope\
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);\
    TRACE("[%s] Took %llu ns/%f ms\n", (name), end.tv_nsec - start.tv_nsec,\
                                             ((r32)end.tv_nsec - (r32)start.tv_nsec) / 1000000.0);\
}

static i32 cuddle_clamp(i32 value, i32 min, i32 max){
    if(value < min){
        return min;
    } else if (value > max) {
        return max;
    }
    return value;
}

struct String {
    u8* data;
    i32 size;
};

static String cloneString(u8* buffer, u32 size){
    String result = {};
    result.size = size;
    result.data = new u8[size + 1];
    
    while(size){
        result.data[result.size - size] = buffer[result.size - size];
        --size;
    }

    // NOTE(Sarmis) not many uses for the null terminated string
    // internally but its there...
    result.data[result.size] = 0;

    return result;
}

static String cloneString(String string, u32 size){
    return cloneString(string.data, size);
}

static String cloneString(char character){
    String result = {};

    result.data = new u8[2];
    result.data[0] = character;
    result.data[1] = 0;
    result.size = 1;

    return result;
}


static String cloneString(const char* string){
    String result = {};

    u32 size = strlen(string);
    result = cloneString((u8*)string, size);

    return result;
}

static u32 characterFirstOccurence(String string, char character){
    u32 result = 0;
    u8* start = string.data;

    while((start - string.data) < string.size){
        if(*start == character){
            result = start - string.data;
            return result;
        }
        ++start;
    }

    return result;
}

static u32 characterLastOccurence(String string, char character){
    u32 result = 0;
    u8* start = string.data;

    while((start - string.data) < string.size){
        if(*start == character){
            result = start - string.data - 1; // this should probably have a -1
        }
        ++start;
    }

    return result;
}

static i32 icharacterFirstOccurence(String string, char character){
    u32 result = -1;
    u8* start = string.data;

    while((start - string.data) < string.size){
        if(*start == character){
            result = start - string.data;
            return result;
        }
        ++start;
    }

    return result;
}

static i32 icharacterLastOccurence(String string, char character){
    u32 result = -1;
    u8* start = string.data;

    while((start - string.data) < string.size){
        if(*start == character){
            result = start - string.data - 1; // this should probably have a -1
        }
        ++start;
    }

    return result;
}

static String cloneStringNoGap(String string){
    String result = {};

    i32 first = icharacterFirstOccurence(string, '\0');
    i32 last = icharacterLastOccurence(string, '\0');

    if(first == -1){
        // no 0's, just clone
        result = cloneString(string, string.size);
    } else {
        result.size = string.size - (last - first);
        
        result.data = new u8[result.size + 1];
        result.data[result.size] = '\0';
        
        // copy before first
        int i;
        for(i = 0; i < first; ++i){
            result.data[i] = string.data[i];
        }
        
        // copy after last
        for(int ii = last + 1; ii < string.size; ++ii){
            result.data[i++] = string.data[ii];
        }
    }
    
    return result;
}



static String subString(String string, u32 start, u32 end){
    String result = {};
    result = cloneString(&string.data[start], end - start);
    return result;
}

static String subString(String string, char delimiter){
    String result = {};
    u8* start = string.data;
    u32 size = 0;
    while(*start != delimiter && size < string.size){
        ++size;
        ++start;
    }

    result = cloneString(string.data, size);
    return result;
}

static String concatenateStrings(const char* A, const char* B){
    String result = {};
    
    u32 sizeA = strlen(A);
    u32 sizeB = strlen(B);

    u32 size = sizeA + sizeB;

    result.size = size;
    result.data = new u8[size + 1];

    for(int i = 0; i < sizeA; ++i){
        result.data[i] = A[i];
    }

    for(int i = 0; i < sizeB; ++i){
        result.data[sizeA + i] = B[i];
    }

    result.data[result.size] = 0;

    return result;
}


static String concatenateStrings(String A, String B){
    // TODO(Sarmis) this will need to be changed
    // if the null terminator from the strings will be removed
    return concatenateStrings((char*)A.data, (char*)B.data);
}

// TODO(Sarmis) maybe move to some memory.h
template<typename T>
struct Buffer {
    u32 currentAmount;
    u32 capacity;
    T* array;
    
    T* begin() { return array; };
    T* end() { return &array[currentAmount - 1]; };
    const T* begin() const { return array; };
    const T* end() const{ return &array[currentAmount - 1]; };
    
    T& operator[](int index){ return array[index]; }
};

template<typename T>
static inline T bufferPop(Buffer<T>* buffer){
    T result = {};
    if(buffer->currentAmount){
        result = buffer->array[buffer->currentAmount - 1];
        --buffer->currentAmount;
    }
    return result;
}

template<typename T>
static inline void bufferAppend(Buffer<T>* buffer, T* entry){
    if(!buffer){
        TRACE("Cannot strech buffer of NULL buffer\n");
        ASSERT(false);
    }
    
    if(buffer->currentAmount == buffer->capacity){
        buffer->capacity = (buffer->capacity + 1) * 2;
        T* clone = buffer->array;
        buffer->array = new T[buffer->capacity];
        if(clone){
            for(int i = 0; i < buffer->currentAmount; ++i){
                buffer->array[i] = clone[i];
            }
            delete[] clone;
        }
    }

    buffer->array[buffer->currentAmount] = *entry;
    ++buffer->currentAmount;
}

template<typename T>
static inline void bufferAppend(Buffer<T>* buffer, Buffer<T>* addition){
    if(!buffer){
        TRACE("Cannot strech buffer of NULL buffer\n");
        ASSERT(false);
    }

    if(buffer->currentAmount + addition->currentAmount >= buffer->capacity){
        buffer->capacity = (buffer->capacity) * 2 + addition->currentAmount;
        T* clone = buffer->array;
        buffer->array = new T[buffer->capacity];
        if(clone){
            for(int i = 0; i < buffer->currentAmount; ++i){
                buffer->array[i] = clone[i];
            }
            delete[] clone;
        }
    }

    if(!addition){
        TRACE("Cannot copy from a NULL buffer\n");
        ASSERT(false);
    }

    for(int i = 0; i < addition->currentAmount; ++i){
        buffer->array[buffer->currentAmount] = addition->array[i];
        ++buffer->currentAmount;
    }    
}

template<typename T>
static inline void bufferAppend(Buffer<T>* buffer, T entry){
    bufferAppend<T>(buffer, &entry);
}

template<typename T>
static inline void bufferAppendAt(Buffer<T>* buffer, T* entry, u32 index){
    if(!buffer){
        TRACE("Cannot strech buffer of NULL buffer\n");
        ASSERT(false);
    }

    ASSERT(index < buffer->currentAmount);
    T* clone = buffer->array;
    bool changed = false;    
    if(buffer->currentAmount == buffer->capacity){
        buffer->capacity = (buffer->capacity + 1) * 2;
        buffer->array = new T[buffer->capacity];
    }
    changed = buffer->array != clone;

    if(clone){
        if(clone != buffer->array){
            for(int i = 0; i < index; ++i){
                buffer->array[i] = clone[i];
            }

            for(int i = 1; i <= buffer->currentAmount - index; ++i){
                buffer->array[index + i] = clone[index - 1 + i];
            }
        } else {
            for(int i = buffer->currentAmount; i > index; --i){
                buffer->array[i] = buffer->array[i - 1];
            }
        }
    }

    if(changed){
        delete[] clone;
    }

    buffer->array[index] = *entry;
    ++buffer->currentAmount;
}



template<typename T>
static inline void bufferRemove(Buffer<T>* buffer, u32 start, u32 end){
    if(!buffer){
        TRACE("Cannot remove from a NULL buffer\n");
        ASSERT(false);
    }

    ASSERT(end < buffer->currentAmount && start <= end);

    for(int i = start; i < end; ++i){
        if(end + i >= buffer->currentAmount){
            break;
        }
        buffer->array[i] = buffer->array[end + i];
        ++start;
    }

    buffer->currentAmount -= end - start;
}

template<typename T>
static inline void bufferRemove(Buffer<T>* buffer, u32 index){
    if(!buffer){
        TRACE("Cannot remove from a NULL buffer\n");
        ASSERT(false);
    }

    ASSERT(index < buffer->currentAmount);

    for(int i = index; i < buffer->currentAmount - 1; ++i){
        buffer->array[i] = buffer->array[i + 1];
    }
    --buffer->currentAmount;
}

template<typename T>
static inline void bufferCleanSafe(Buffer<T>* buffer){
    buffer->array = NULL;
    buffer->capacity = 0;
    buffer->currentAmount = 0;
}

template<typename T>
static inline void bufferClean(Buffer<T>* buffer){
    if(buffer->array){
        delete[] buffer->array;
        bufferCleanSafe<T>(buffer);
    }
}

static String operator+(String left, const char* right){
    return concatenateStrings((char*)left.data, right);
}

static String operator+(const char* left, String right){
    return concatenateStrings(left, (char*)right.data);
}

static String operator+(String left, String right){
    return concatenateStrings(left, right);
}

static void operator+=(String& left, String right){
    left = left + right;
}

static bool operator==(String& left, const char* right){
    i32 rlen = strlen(right);

    if(left.size != rlen){
        return false;
    }

    u8* clone = left.data;

    while(*right){
        if(*clone++ != *right++){
            return false;
        }
    }
    return true;
}

static bool operator==(String& left, String& right){
    return left == (char*)right.data;
}

static bool operator!=(String& left, const char* right){
    return !(left == right);
}

static void skipSpacesInBuffer(u8*& buffer){
    while(*buffer == ' ' || 
          *buffer == '\t'){
    
        ++buffer;
    }
}

static u32 readU32FromBuffer(u8* buffer){
    u32 result = 0;
    skipSpacesInBuffer(buffer);

    u8 character = *buffer++;

    while((character >= '0' && character <= '9')) {
        result *= 10;
        result += character - '0';
        character = *buffer++;
    }

    --buffer;

    return result;
}


static inline bool isSpacingCharacter(char character){
    if(character == '\n' || character == '\r' ||
       character == '\t' || character ==  ' '){
        return true;
    }
    return false;
}

static inline bool isSymbolCharacter(char character){
    if(character == '`' || character == '~' || character == '!' ||
       character == '@' || character == '#' || character == '$' ||
       character == '%' || character == '^' || character == '&' ||
       character == '*' || character == '(' || character == ')' ||
       character == '-' || character == '=' || character == '+' ||
       character == '_' || character == '[' || character == ']' ||
       character == '\\' || character == '{' || character == '}' ||
       character == ';' || character == '\'' || character == ':' ||
       character == '"' || character == ',' || character == '.' || character == '|' ||
       character == '/' || character == '<' || character == '>' || character == '?'){
           return true;
    }
    return false;
}

static inline bool isAlphabeticalCharacter(char character){
    if((character >= 'a' && character <= 'z') ||
       (character >= 'A' && character <= 'Z')){

        return true;
    }
    return false;
}

static inline bool isAlphanumericCharacter(char character){
    if((character >= 'a' && character <= 'z') ||
       (character >= 'A' && character <= 'Z') ||
       (isSymbolCharacter(character) ||
       (character >= '0' && character <= '9'))){

        return true;
       }
    return false;
}


// NOTE(Sarmis) ONLY USE ON CLONED STRINGS
//              NOT ON REFERENCES
static void cleanStringBuffer(Buffer<String> buffer){
    for(int i = 0; i < buffer.currentAmount; ++i){
        if(buffer[i].data){
            buffer[i].size = 0;
            delete[] buffer[i].data;
        }
    }
    
    bufferClean<String>(&buffer);
}

static Buffer<String> split(String string, char delimiter){
    Buffer<String> result = {};
    
    i32 start = 0;
    i32 end = 0;

    for(int i = 0; i <= string.size; ++i){
         if(string.data[i] == delimiter){
            end = i;
            if(end - start <= 1){
                start = i;
                continue;
            }
            bufferAppend<String>(&result, subString(string, start ? (start + 1) : start, end));
            start = i;
         } else if(string.data[i] == '\0'){
             end = i;
             if(end - start <= 1){
                 start = i;
                 continue;
             }
             bufferAppend<String>(&result, subString(string, start ? (start + 1) : start, end));
             start = i;
         }
    }

    return result;
}


static bool stringIsMatchingInWord(String substring, String word){
    for(int i = 0; i < word.size - substring.size; ++i){
        bool match = true;
        for(int ii = 0; ii < substring.size; ++ii){
            if(substring.data[ii] != word.data[i + ii]){
                match = false;
                break;
            }
        }
        if(match){
            return true;
        }
    }
   return false;
}


static bool stringIsPartiallyMatching(char* substring, char* string){
    while(*substring){
        if(*substring++ != *string++){
            return false;
        }
    }

    return true;
}

static bool stringIsPartiallyMatching(String substring, char* string){
    char* clone = (char*)substring.data;
    while(substring.size--){
        if(*clone++ != *string++){
            return false;
        }
    }

    return true;
}

