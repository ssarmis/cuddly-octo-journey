#pragma once

#include "general.h"

#ifdef SDL_PLATFORM_BACKEND
#include <SDL2/SDL.h>

struct PlatformWindow {
    SDL_Window* nativeWindow;
};

static PlatformWindow platformCreateWindow(char* name, u32 width, u32 height){
    PlatformWindow result = {};
    
    result.nativeWindow = SDL_CreateWindow(name,
                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    width, height,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(result.nativeWindow);

    return result;
}

static void platformInitializeParameters(){
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        TRACE("Could not initialize SDL2\n");
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

}

#endif
