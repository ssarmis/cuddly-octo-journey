#pragma once
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


struct String {
    u8* data;
    i32 size;
};

String cloneString(u8* buffer, u32 size){
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

String cloneString(String string, u32 size){
    return cloneString(string.data, size);
}

String cloneString(const char* string){
    String result = {};

    u32 size = strlen(string);
    result = cloneString((u8*)string, size);

    return result;
}

u32 characterFirstOccurence(String string, char character){
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

u32 characterLastOccurence(String string, char character){
    u32 result = 0;
    u8* start = string.data;

    while((start - string.data) < string.size){
        if(*start == character){
            result = start - string.data;
        }
        ++start;
    }

    return result;
}


String subString(String string, u32 start, u32 end){
    String result = {};
    result = cloneString(&string.data[start], end - start);
    return result;
}

String subString(String string, char delimiter){
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

String concatenateStrings(const char* A, const char* B){
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


String concatenateStrings(String A, String B){
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

    if(buffer->currentAmount == buffer->capacity){
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

String operator+(String left, const char* right){
    return concatenateStrings((char*)left.data, right);
}

String operator+(const char* left, String right){
    return concatenateStrings(left, (char*)right.data);
}

String operator+(String left, String right){
    return concatenateStrings(left, right);
}

void operator+=(String& left, String right){
    left = left + right;
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


bool isSpacingCharacter(char character){
    if(character == '\n' || character == '\r' ||
       character == '\t' || character ==  ' '){
        return true;
    }
    return false;
}

bool isSymbolCharacter(char character){
    if(character == '`' || character == '~' || character == '!' ||
       character == '@' || character == '#' || character == '$' ||
       character == '%' || character == '^' || character == '&' ||
       character == '*' || character == '(' || character == ')' ||
       character == '-' || character == '=' || character == '+' ||
       character == '_' || character == '[' || character == ']' ||
       character == '\\' || character == '{' || character == '}' ||
       character == ';' || character == '\'' || character == ':' ||
       character == '"' || character == ',' || character == '.' ||
       character == '/' || character == '<' || character == '>' || character == '?'){
           return true;
    }
    return false;
}

bool isAlphanumericCharacter(char character){

    if((character >= 'a' && character <= 'z') ||
       (character >= 'A' && character <= 'Z') ||
       (isSymbolCharacter(character) ||
       (character >= '0' && character <= '9'))){
           return true;
       }
    return false;
}
