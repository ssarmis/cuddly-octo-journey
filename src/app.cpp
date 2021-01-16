#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation

#ifdef __APPLE__
// NOTE(Sarmis) STFU ?
#define GL_SILENCE_DEPRECATION
#endif
#include <SDL2/SDL.h>

#include <unistd.h>
#include <errno.h>

#include "gl.h"
#include "gl_utilities.h"
#include "render_buffer.h"
#include "math.h"
#include "shader.h"
#include "font.h"

#include "settings.h"
#include "gap_buffer.h"
#include "window.h"
#include "panel.h"
#include "panel_actions.h"
#include "panel_updates.h"
#include "keyboard_manager.h"
#include "window_bindings.h"
#include "layout_manager.h"
#include "event_manager.h"

#include "app.h"

#include <time.h>

#define TURBO_TIME 1

ApplicationLayoutData applicationLayoutData = {};

String findFont(char* name){
#ifdef __unix__
    char commandString[256] = {};
    char commandOutput[512] = {};

    strcat(commandString, "fc-match --format=%{file} ");
    strcat(commandString, name);
    
    FILE* command = popen(commandString, "r");

    ASSERT(command);

    fgets(commandOutput, 512, command);

    pclose(command);
    String result = cloneString(commandOutput);
    return result;
#else
    String result = cloneString("LiberationMono-Regular.ttf");
    return result;
#endif
}



int main(int argumentCount, char* arguments[]){

#if 1
    // TODO(Sarmis) only load file contents to a certain depth
    // so not everything is loaded
    applicationLayoutData.scheduleChangeInSize = false;

    applicationLayoutData.windowWidth = 1280;
    applicationLayoutData.windowHeight = 768;

    applicationLayoutData.windowCount = 2;
    applicationLayoutData.windows[0] = windowCreate(applicationLayoutData.windowWidth / 2,
                                                    applicationLayoutData.windowHeight, 0, 0);
    applicationLayoutData.windows[1] = windowCreate(applicationLayoutData.windowWidth / 2,
                                                    applicationLayoutData.windowHeight, applicationLayoutData.windowWidth / 2 + 2, 0);

    applicationLayoutData.layoutWindows[0] = applicationLayoutData.windows[0]; 
    applicationLayoutData.layoutWindows[1] = applicationLayoutData.windows[1];

    applicationLayoutData.currentWindowIndex = 0;
    applicationLayoutData.currentWindow = &applicationLayoutData.windows[0];
    applicationLayoutData.currentBuffer = &applicationLayoutData.currentWindow->buffer;


    gapInsertNullTerminatedStringAt(&applicationLayoutData.windows[1].buffer, R"(
********************************************************************

                    uwu Welcome to my editor

            I hope you won't get annoyed by the bugs ;)
        it has some basic functionalities:
            - basic C/C++ syntax highlighting (static, int, void, etc...)
              not customizeable yet without the source code
            - 2 windows, one can be closed, the other can be opened again
              with CTRL + SHIFT + UP,
              any number of windows is supported with any positioning
              but I don't have a proper grid system in place right now
            - basic UNDO
            - find/goto line
            - highlights on words

        Thank you for using me *chu* <3

********************************************************************

    )", 0);
    
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

    String fontPath = findFont("LiberationMono-Regular.ttf");

    FontGL font = createFont((char*)fontPath.data);

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

	
    if(argumentCount > 1){
        applicationLayoutData.currentWindow->buffer = gapReadFile(arguments[1]);
    } else {
        applicationLayoutData.filePool = editorFilePoolLoadAllFilesFromDirectory(".", true);
        applicationLayoutData.currentWindow->buffer = gapCreateEmpty();
    }

    // TODO(Sarmis) make initialization for these
    // TODO(Sarmis) using virtual functions would yield
    //              a similar result but with les hussle

    applicationLayoutData.panelGroup.quickOpenPanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Quick open");
    applicationLayoutData.panelGroup.quickOpenPanel.action = quickOpenFileAction;
    applicationLayoutData.panelGroup.quickOpenPanel.tick = quickOpenFileTick;

    applicationLayoutData.panelGroup.openFilePanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Open file");
    applicationLayoutData.panelGroup.openFilePanel.action = openFileAction;
    applicationLayoutData.panelGroup.openFilePanel.tick = openFileTick;

    applicationLayoutData.panelGroup.findPanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Find");
    applicationLayoutData.panelGroup.findPanel.action = findAction;
    applicationLayoutData.panelGroup.findPanel.tick = findTick;

    applicationLayoutData.panelGroup.gotoLinePanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Goto line");
    applicationLayoutData.panelGroup.gotoLinePanel.action = gotoLineAction;

    applicationLayoutData.panelGroup.saveFilePanel = panelCreate({0, 0, 0}, {400, FONT_HEIGHT * 3 + 12 + 4}, "Save file");
    applicationLayoutData.panelGroup.saveFilePanel.action = saveFileAction;

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

    editorWindowKeyBindingInitialize(&windowKeyboardBindingManager);
    layoutKeyBindingInitialize(&layoutManagerKeybindings);

    LayoutEvent layoutEvent = {};


    bool done = false;
    while(!done){
        time++;
        time %= 40;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        eventTick(&done, &layoutEvent, &keyboardManager);
        layoutManagerTick(&layoutEvent, &applicationLayoutData, &keyboardManager);

        // maybe the schedule will be a list of changes question mark ?
        if(applicationLayoutData.scheduleChangeInSize){
            SHADER_SCOPE(shader.programId, {
                shaderSetUniform4m(shader.locations.matrixPerspective, 
                                orthographic(0, applicationLayoutData.windowWidth, 0, applicationLayoutData.windowHeight));
            });

            SHADER_SCOPE(shaderUI.programId, {
                shaderSetUniform4m(shaderUI.locations.matrixPerspective, 
                                orthographic(0, applicationLayoutData.windowWidth, 0, applicationLayoutData.windowHeight));
            });

            applicationLayoutData.scheduleChangeInSize = false;
        }

        v2 uvs[4] = {};

        glViewport(0, 0, applicationLayoutData.windowWidth, applicationLayoutData.windowHeight);

        glClearColor(0.3, 0.3, 0.3, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font.textureId);

        // TODO(Sarmis) maybe only tick the window if a panel is active
        // or just tick the panels first and consume the keystroke if
        // it was used in a tick

        // debugging
        // char buffer[(1 << 10) * 100];
        // gapClean(&applicationLayoutData.windows[1].buffer);
        // applicationLayoutData.windows[1].buffer = gapCreateEmpty();
        // gapPrintGap(buffer, &applicationLayoutData.windows[0].buffer);
        // gapInsertNullTerminatedStringAt(&applicationLayoutData.windows[1].buffer, buffer, 0);
        //


        for(int i = 0; i < applicationLayoutData.windowCount; ++i){
            if(applicationLayoutData.windows[i].visible){
                if(i == applicationLayoutData.currentWindowIndex && !applicationLayoutData.panelGroup.panel.active){
                    editorWindowTick(&applicationLayoutData.windows[i], &keyboardManager);
                }
                editorWindowRender(&applicationLayoutData.windows[i], 
                            &shader, &shaderUI,
                            &renderBuffer, &renderBufferUI, &renderBufferBackground,
                            &font,
                            time, applicationLayoutData.currentWindowIndex == i);
            }
        }
        
        if(applicationLayoutData.panelGroup.panel.active){
            applicationLayoutData.panelGroup.panel.tick(&applicationLayoutData, &keyboardManager);
            if(applicationLayoutData.panelGroup.panel.active){ // could become inactive in ticking
                panelRender(&applicationLayoutData.panelGroup.panel, applicationLayoutData.currentWindow,
                    &shader, &shaderUI,
                    &renderBuffer, &renderBufferUI, &renderBufferBackground,
                    &font,
                    time, applicationLayoutData.windowWidth, applicationLayoutData.windowHeight);
            }
        } else if(applicationLayoutData.currentBuffer == &applicationLayoutData.panelGroup.panel.buffer){
            applicationLayoutData.currentBuffer = &applicationLayoutData.currentWindow->buffer;
        }
        
        SDL_GL_SwapWindow(window);
    }

    bufferClean<Selection>(&applicationLayoutData.windows[0].selections);
    editorFilePoolFreeSpace(&applicationLayoutData.filePool);

#endif
    return 0;
}


  
