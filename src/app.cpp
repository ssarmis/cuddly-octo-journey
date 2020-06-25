#include <SDL2/SDL.h>

#include "gl.h"
#include "gl_utilities.h"
#include "render_buffer.h"
#include "math.h"
#include "shader.h"
#include "font.h"

#include "gap_buffer.h"
#include "window.h"
#include "panel.h"
#include "panel_actions.h"
#include "panel_updates.h"
#include "keyboard_manager.h"
#include "layout_manager.h"
#include "event_manager.h"

#include "app.h"

#include <time.h>

#define TURBO_TIME 1

ApplicationLayoutData applicationLayoutData = {};

int main(int argumentCount, char* arguments[]){

    applicationLayoutData.windowWidth = 1280;
    applicationLayoutData.windowHeight = 768;

    applicationLayoutData.windowCount = 2;
    applicationLayoutData.windows[0] = windowCreate(applicationLayoutData.windowWidth / 2,
                                                    applicationLayoutData.windowHeight, 0, 0);
    applicationLayoutData.windows[1] = windowCreate(applicationLayoutData.windowWidth / 2,
                                                    applicationLayoutData.windowHeight, applicationLayoutData.windowWidth / 2, 0);

    applicationLayoutData.currentWindowIndex = 0;
    applicationLayoutData.currentWindow = &applicationLayoutData.windows[0];
    applicationLayoutData.currentBuffer = &applicationLayoutData.currentWindow->buffer;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        TRACE("Could not initialize SDL2\n");
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window *window = SDL_CreateWindow("App",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          applicationLayoutData.windowWidth, applicationLayoutData.windowHeight,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    initializeGL();

    TRACE("%s\n", glGetString(GL_VERSION));

    SDL_Event event;

    RenderBuffer renderBuffer = createVertexArrayObject();
    pushPreProcesedQuadsIndices(&renderBuffer);

    RenderBuffer renderBufferBackground = createVertexArrayObject();
    RenderBuffer renderBufferUI = createVertexArrayObject();

    FontGL font = createFont();
    
    Shader shader = createShader();
    Shader shaderUI = createShaderCursor();

    SHADER_SCOPE(shader.programId, {
        shaderSetUniform4m(shader.locations.matrixPerspective, 
                           orthographic(0, applicationLayoutData.windowWidth, 0, applicationLayoutData.windowHeight));
    });

    SHADER_SCOPE(shaderUI.programId, {
        shaderSetUniform4m(shaderUI.locations.matrixPerspective, 
                           orthographic(0, applicationLayoutData.windowWidth, 0, applicationLayoutData.windowHeight));
    });

    bool done = false;


    // GapBuffer buffer = {};
    // buffer.data = new u8[GAP_DEFAULT_SIZE];
    // memset(buffer.data, 0, GAP_DEFAULT_SIZE);
    // buffer.size = GAP_DEFAULT_SIZE;
    // buffer.cursor = 0;
    // buffer.gap.start = 0;
    // buffer.gap.end = GAP_DEFAULT_SIZE;

    if(argumentCount > 1){
        applicationLayoutData.currentWindow->buffer = gapReadFile(arguments[1]);
    } else {
        applicationLayoutData.currentWindow->buffer = gapCreateEmpty();
    }

    // TODO(Sarmis) make initialization for these
    applicationLayoutData.panelGroup.openFilePanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Open file");
    applicationLayoutData.panelGroup.openFilePanel.action = openFileAction;
    applicationLayoutData.panelGroup.openFilePanel.tick = openFileTick;

    applicationLayoutData.panelGroup.findPanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Find");
    applicationLayoutData.panelGroup.findPanel.action = findAction;

    applicationLayoutData.panelGroup.gotoLinePanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Goto line");
    applicationLayoutData.panelGroup.gotoLinePanel.action = gotoLineAction;

    applicationLayoutData.panelGroup.saveFilePanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Save file");
    applicationLayoutData.panelGroup.saveFilePanel.action = saveFileAction;

#if 1
    KeyboardManager keyboardManager = {};

    for(char c = 'a'; c <= 'z'; ++c){
        keyboardManager.shiftCharactersLUT[c] = c - ' ';
    }

    keyboardManager.shiftCharactersLUT['1'] = '!';
    keyboardManager.shiftCharactersLUT['2'] = '@';
    keyboardManager.shiftCharactersLUT['3'] = '#';
    keyboardManager.shiftCharactersLUT['4'] = '$';
    keyboardManager.shiftCharactersLUT['5'] = '%';
    keyboardManager.shiftCharactersLUT['6'] = '^';
    keyboardManager.shiftCharactersLUT['7'] = '&';
    keyboardManager.shiftCharactersLUT['8'] = '*';
    keyboardManager.shiftCharactersLUT['9'] = '(';
    keyboardManager.shiftCharactersLUT['0'] = ')';
    keyboardManager.shiftCharactersLUT['['] = '{';
    keyboardManager.shiftCharactersLUT[']'] = '}';
    keyboardManager.shiftCharactersLUT['\''] = '"';
    keyboardManager.shiftCharactersLUT['\\'] = '|';
    keyboardManager.shiftCharactersLUT['/'] = '?';
    keyboardManager.shiftCharactersLUT[','] = '<';
    keyboardManager.shiftCharactersLUT['.'] = '>';
    keyboardManager.shiftCharactersLUT['='] = '+';
    keyboardManager.shiftCharactersLUT['-'] = '_';
    keyboardManager.shiftCharactersLUT[';'] = ':';

    i32 time = 0;

    // TODO(Sarmis) since this is only stuff relating window
    // key binds, the function should be named and placed better
    keyBindingInitialize(&windowKeyboardBindingManager);
    layoutKeyBindingInitialize(&layoutManagerKeybindings);

    while(!done){
        time++;
        time %= 20;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // TODO(Sarmis) make something to hold this "done"
        eventTick(&done, &keyboardManager);

        // printf("%x\n", keyboardManager.currentActiveKeyStroke);
        // if(keyboardManager.currentActiveKeyStroke == (KEY_CTRL | KEY_TAB)){
        // }

        layoutManagerTick(&applicationLayoutData, &keyboardManager);

        v2 uvs[4] = {};

        glViewport(0, 0, applicationLayoutData.windowWidth, applicationLayoutData.windowHeight);

        glClearColor(DEFAULT_COLOR_BACKGROUND.x, DEFAULT_COLOR_BACKGROUND.y, DEFAULT_COLOR_BACKGROUND.z, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font.textureId);

        // TODO(Sarmis) maybe only tick the window if a panel is active
        // or just tick the panels first and consume the keystroke if
        // it was used in a tick

        for(int i = 0; i < applicationLayoutData.windowCount; ++i){
            if(i == applicationLayoutData.currentWindowIndex && !applicationLayoutData.panelGroup.panel.active){
                editorWindowTick(&applicationLayoutData.windows[i], &keyboardManager);
            }
            editorWindowRender(&applicationLayoutData.windows[i], 
                        &shader, &shaderUI,
                        &renderBuffer, &renderBufferUI, &renderBufferBackground,
                        &font,
                        time, applicationLayoutData.currentWindowIndex == i);
        }
        
        if(applicationLayoutData.panelGroup.panel.active){
            applicationLayoutData.panelGroup.panel.tick(&applicationLayoutData.panelGroup.panel,
                                                        applicationLayoutData.currentWindow, &keyboardManager);
            panelRender(&applicationLayoutData.panelGroup.panel, applicationLayoutData.currentWindow,
                 &shader, &shaderUI,
                 &renderBuffer, &renderBufferUI, &renderBufferBackground,
                 &font,
                 time, applicationLayoutData.windowWidth, applicationLayoutData.windowHeight);
        }
        
        SDL_GL_SwapWindow(window);
    }
#endif
    return 0;
}


