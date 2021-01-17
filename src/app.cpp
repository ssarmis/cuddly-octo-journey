#define SDL_PLATFORM_BACKEND
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

#include "platform_window.h"
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
    platformInitializeParameters();
    
    applicationLayoutDataInitialize(&applicationLayoutData);
    PlatformWindow window = platformCreateWindow("cuddle", applicationLayoutData.windowWidth, applicationLayoutData.windowHeight);

    if(argumentCount > 1){
        applicationLayoutData.currentWindow->currentFile->buffer = gapReadFile(arguments[1]);
    } else {
        applicationLayoutData.filePool = editorFilePoolLoadAllFilesFromDirectory(".", true);
    }

    initializeGL();

    TRACE("%s\n", glGetString(GL_VERSION));

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

    KeyboardManager keyboardManager = {};
    keyboardManagerInitialize(&keyboardManager);

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
            applicationLayoutData.currentBuffer = &applicationLayoutData.currentWindow->currentFile->buffer;
        }
        
        SDL_GL_SwapWindow(window.nativeWindow);
    }

    bufferClean<Selection>(&applicationLayoutData.windows[0].selections);
    editorFilePoolFreeSpace(&applicationLayoutData.filePool);

#endif
    return 0;
}


  
