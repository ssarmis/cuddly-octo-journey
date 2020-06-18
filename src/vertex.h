#pragma once
#include "general.h"
#include "math.h"

#pragma pack(push, 1)
struct Vertex {
    union {
        v3 position;
        v3 xyz;
        struct {
            r32 x;
            r32 y;
            r32 z;
        };
    };

    union {
        v2 uv;
        struct {
            r32 u;
            r32 v;
        };
    };

    union {
        v3 color;
        v3 rgb;
        struct {
            r32 r;
            r32 g;
            r32 b;
        };
    };


    Vertex():
    position(0), uv(0), color(0){
    }

    Vertex(v3 position):
    position(position), uv(0), color(0){
    }

    Vertex(v3 position, v2 uv):
    position(position), uv(uv), color(0){
    }

    Vertex(v3 position, v2 uv, v3 color):
    position(position), uv(uv), color(color){
    }
};
#pragma pack(pop)