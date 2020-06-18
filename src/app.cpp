#include <SDL2/SDL.h>

#include "gl.h"
#include "gl_utilities.h"
#include "render_buffer.h"
#include "math.h"
#include "shader.h"
#include "font.h"

#include "gap_buffer.h"
#include "linked_lines.h"
#include "selection.h"

#include <time.h>

#define TURBO_TIME 1

int windowWidth = 1280;
int windowHeight = 768;
u32 windowTop = 0;
u32 windowBottom = windowHeight;
m4 windowTransform = m4();
m4 windowView = m4();

int main(){
#if 1
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
                                          windowWidth, windowHeight,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    initializeGL();

    TRACE("%s\n", glGetString(GL_VERSION));

    SDL_Event event;

    RenderBuffer renderBuffer = createVertexArrayObject();
    // preprocess indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderBuffer.indexBufferId);
    Index* indices = NULL;

    indices = (Index*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

    Index* clone = indices;
    Index offset = 0;
    for(int i = 0; i < renderBuffer.indexBufferSize / 32; ++i){
        *indices++ = offset + 0;
        *indices++ = offset + 1;
        *indices++ = offset + 2;

        *indices++ = offset + 1;
        *indices++ = offset + 3;
        *indices++ = offset + 2;
        
        offset += 4;
    }

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //

    RenderBuffer renderBufferBackground = createVertexArrayObject();
    RenderBuffer renderBufferUI = createVertexArrayObject();

    FontGL font = createFont();
    
    Shader shader = createShader();
    Shader shaderUI = createShaderCursor();

    SHADER_SCOPE(shader.programId, {
        shaderSetUniform4m(shader.locations.matrixPerspective, 
                           orthographic(0, windowWidth, 0, windowHeight));
    });

    SHADER_SCOPE(shaderUI.programId, {
        shaderSetUniform4m(shaderUI.locations.matrixPerspective, 
                           orthographic(0, windowWidth, 0, windowHeight));
    });

    bool done = false;

    bool backspaceReleased = true;
    bool homeReleased = true;
    bool endReleased = true;
    bool arrowReleased[4] = {true, true, true, true};
    bool tabReleased = true;
    bool newlineReleased = true;
    bool pageDownReleased = true;
    bool pageUpReleased = true;

    bool controlSeeking = false;

    bool backspaceTurbo = false;
    bool arrowTurbo = false;

    u32 backspaceTime = 0;
    u32 arrowTime = 0;

    v3 cursor = {};
    i32 cursorPointer = 0;
#endif

    // GapBuffer buffer = {};
    // buffer.data = new u8[GAP_DEFAULT_SIZE];
    // memset(buffer.data, 0, GAP_DEFAULT_SIZE);
    // buffer.size = GAP_DEFAULT_SIZE;
    // buffer.cursor = 0;
    // buffer.gap.start = 0;
    // buffer.gap.end = GAP_DEFAULT_SIZE;

    GapBuffer buffer = gapReadFile("src/app.cpp");

#if 1
    bool shiftPressed = false;

    char shiftCharactersLUT[256];
    for(char c = 'a'; c <= 'z'; ++c){
        shiftCharactersLUT[c] = c - ' ';
    }

    shiftCharactersLUT['1'] = '!';
    shiftCharactersLUT['2'] = '@';
    shiftCharactersLUT['3'] = '#';
    shiftCharactersLUT['4'] = '$';
    shiftCharactersLUT['5'] = '%';
    shiftCharactersLUT['6'] = '^';
    shiftCharactersLUT['7'] = '&';
    shiftCharactersLUT['8'] = '*';
    shiftCharactersLUT['9'] = '(';
    shiftCharactersLUT['0'] = ')';
    shiftCharactersLUT['['] = '{';
    shiftCharactersLUT[']'] = '}';
    shiftCharactersLUT['\''] = '"';
    shiftCharactersLUT['\\'] = '|';
    shiftCharactersLUT['/'] = '?';
    shiftCharactersLUT[','] = '<';
    shiftCharactersLUT[','] = '<';
    shiftCharactersLUT['='] = '+';
    shiftCharactersLUT['-'] = '_';

    i32 time = 0;

    while(!done){
        time++;
        time %= 20;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_WINDOWEVENT: {
                        switch(event.window.event){
                            case SDL_WINDOWEVENT_SIZE_CHANGED: {
                                    windowWidth = event.window.data1;
                                    windowHeight = event.window.data2;
                                    // TODO(Sarmis) I have to fix these 
                                    // depending on the new size,
                                    // the already existing offset needs to 
                                    // update
                                    windowBottom = windowHeight;

                                    SHADER_SCOPE(shader.programId, {
                                        shaderSetUniform4m(shader.locations.matrixPerspective, 
                                                        orthographic(0, windowWidth, 0, windowHeight));
                                    });

                                    SHADER_SCOPE(shaderUI.programId, {
                                        shaderSetUniform4m(shaderUI.locations.matrixPerspective, 
                                                        orthographic(0, windowWidth, 0, windowHeight));
                                    });
                                }
                                break;
                        }
                    }
                    break;

                case SDL_KEYUP: {
                        switch (event.key.keysym.scancode) {
                            case SDL_SCANCODE_LCTRL: case SDL_SCANCODE_RCTRL: {
                                    controlSeeking = false;
                                }
                                break;

                            case SDL_SCANCODE_PAGEUP: {
                                    pageUpReleased = true;
                                }
                                break;
    
                            case SDL_SCANCODE_PAGEDOWN: {
                                    pageDownReleased = true;
                                }
                                break;

                            case SDL_SCANCODE_HOME: {
                                    homeReleased = true;
                                }
                                break;

                            case SDL_SCANCODE_END: {
                                    endReleased = true;
                                }
                                break;

                            case SDL_SCANCODE_LSHIFT: case SDL_SCANCODE_RSHIFT: {
                                    shiftPressed = false;
                                }
                                break;

                            case SDL_SCANCODE_TAB: {
                                    tabReleased = true;
                                }
                                break;

                            case SDL_SCANCODE_LEFT:{
                                    arrowReleased[3] = true;
                                    arrowTurbo = false;
                                    arrowTime = 0;
                                } 
                                break;

                            case SDL_SCANCODE_RIGHT: {
                                    arrowReleased[2] = true;
                                    arrowTurbo = false;
                                    arrowTime = 0;
                                }
                                break;

                            case SDL_SCANCODE_UP:{
                                    arrowReleased[1] = true;
                                    arrowTurbo = false;
                                    arrowTime = 0;
                                }
                            
                            case SDL_SCANCODE_DOWN: {
                                    arrowReleased[0] = true;
                                    arrowTurbo = false;
                                    arrowTime = 0;
                                }
                                break;

                            case SDL_SCANCODE_DELETE: case SDL_SCANCODE_BACKSPACE: {
                                    backspaceReleased = true;
                                    backspaceTurbo = false;
                                    backspaceTime = 0;
                                }
                                break;

                            case SDL_SCANCODE_RETURN2: case SDL_SCANCODE_RETURN: {
                                    newlineReleased = true;
                                }
                                break;
                        }
                    }
                    break;

                case SDL_KEYDOWN: {
                        switch (event.key.keysym.scancode) {
                            case SDL_SCANCODE_LCTRL: case SDL_SCANCODE_RCTRL: {
                                    controlSeeking = true;
                                }
                                break;

                            case SDL_SCANCODE_LSHIFT: case SDL_SCANCODE_RSHIFT: {
                                    shiftPressed = true;
                                }
                                break;

                            case SDL_SCANCODE_PAGEUP: {
                                    if(pageUpReleased){
                                        u32 heightInLines = windowHeight / FONT_HEIGHT;
                                        gapSeekCursorINewlinesIfPossible(&buffer, -heightInLines);
                                    }
                                    pageUpReleased = false;
                                }
                                break;

                            case SDL_SCANCODE_PAGEDOWN: {
                                    // SPONGE 
                                    // bug with the pagedown when resizing
                                    // after scroll was done
                                    if(pageDownReleased){
                                        u32 heightInLines = windowHeight / FONT_HEIGHT;
                                        gapSeekCursorINewlinesIfPossible(&buffer, heightInLines);
                                    }
                                    pageDownReleased = false;
                                }
                                break;

                            case SDL_SCANCODE_HOME: {
                                    if(homeReleased){
                                        if(shiftPressed){
                                            if(!gapGetSelectionSize(&buffer)){
                                                // there is no current selection alive
                                                buffer.selection.end = buffer.cursor;
                                            }
                                        } else {
                                            buffer.selection.start = buffer.selection.end;
                                        }

                                        gapSeekCursorToPreviousNewline(&buffer);

                                        if(buffer.data[UserToGap(buffer.gap, buffer.cursor)] == '\n'){
                                            gapIncreaseCursor(&buffer);
                                        }

                                        if(shiftPressed){
                                            buffer.selection.start = buffer.cursor;
                                        }
                                    }
                                    homeReleased = false;
                                }
                                break;

                            case SDL_SCANCODE_END: {
                                    if(endReleased){
                                        if(shiftPressed){
                                            if(!gapGetSelectionSize(&buffer)){
                                                // there is no current selection alive
                                                buffer.selection.start = buffer.cursor;
                                            }
                                        } else {
                                            buffer.selection.start = buffer.selection.end;
                                        }

                                        if(buffer.data[UserToGap(buffer.gap, buffer.cursor)] != '\n'){
                                            gapSeekCursorToNewline(&buffer);
                                        }

                                        if(shiftPressed){
                                            buffer.selection.end = buffer.cursor;
                                        }
                                    }
                                    endReleased = false;
                                }
                                break;

                            case SDL_SCANCODE_F12: {
                                }
                                break;

                            case SDL_SCANCODE_TAB: {
                                    u8 tab = '\t';
                                    if(tabReleased){
                                        gapInsertCharacterAt(&buffer, tab, buffer.cursor);
                                        gapIncreaseCursor(&buffer);
                                    }
                                    tabReleased = false;
                                }
                                break;

                            case SDL_SCANCODE_RETURN2: case SDL_SCANCODE_RETURN: {
                                    u8 newline = '\n';
                                    if(newlineReleased){
                                        i32 tabs = gapGetAmontOfTabsBeforeCursor(&buffer);

                                        gapInsertCharacterAt(&buffer, newline, buffer.cursor);
                                        gapIncreaseCursor(&buffer);

                                        while(tabs--){
                                            gapInsertCharacterAt(&buffer, '\t', buffer.cursor);
                                            gapIncreaseCursor(&buffer);
                                        }
                                    }
                                    newlineReleased = false;
                                }
                                break;

                            case SDL_SCANCODE_UP: {
                                    if(arrowReleased[1] || arrowTurbo){
                                        i32 distance = gapGetDistanceFromPreviousNewline(&buffer);

                                        if(shiftPressed){
                                            if(!gapGetSelectionSize(&buffer)){
                                                // there is no current selection alive
                                                buffer.selection.end = buffer.cursor;
                                            }
                                        } else {
                                            buffer.selection.start = buffer.selection.end;
                                        }

                                        if(controlSeeking){
                                            gapSeekCursorINewlinesIfPossible(&buffer, -10);
                                        } else {
                                            gapSeekCursorToPreviousNewline(&buffer);
                                            if(buffer.cursor - 1 >= 0){
                                                i32 distanceOnPreviousLine = gapGetDistanceFromPreviousNewline(&buffer);
                                                distanceOnPreviousLine -= distance;
                                                if(distanceOnPreviousLine >= 0){
                                                    gapSeekCursor(&buffer, -distanceOnPreviousLine);
                                                }
                                            }
                                        }

                                        if(shiftPressed){
                                            buffer.selection.start = buffer.cursor;
                                        }
                                    } else {
                                        arrowTime++;
                                        if (arrowTime >= TURBO_TIME){
                                            arrowTurbo = true;
                                        }
                                    }
                                    arrowReleased[1] = false;
                                }
                                break;

                            case SDL_SCANCODE_DOWN: {
                                    if(arrowReleased[0] || arrowTurbo){
                                        i32 distance = gapGetDistanceFromPreviousNewline(&buffer);

                                        if(shiftPressed){
                                            if(!gapGetSelectionSize(&buffer)){
                                                // there is no current selection alive
                                                buffer.selection.start = buffer.cursor;
                                            }
                                        } else {
                                            buffer.selection.start = buffer.selection.end;
                                        }

                                        if(controlSeeking){
                                            gapSeekCursorINewlinesIfPossible(&buffer, 10);
                                        } else {
                                            gapSeekCursorToNewline(&buffer);

                                            if(buffer.cursor + 1 < buffer.size){
                                                gapIncreaseCursor(&buffer);
                                                i32 lineLength = gapGetDistanceToNewline(&buffer);
                                                if(distance <= lineLength){
                                                    gapSeekCursor(&buffer, distance - 1);
                                                } else {
                                                    gapSeekCursor(&buffer, lineLength);
                                                }
                                            }
                                        }

                                        if(shiftPressed){
                                            buffer.selection.end = buffer.cursor;
                                        }
                                    } else {
                                        arrowTime++;
                                        if (arrowTime >= TURBO_TIME){
                                            arrowTurbo = true;
                                        }
                                    }
                                    arrowReleased[0] = false;
                                }
                                break;

                            case SDL_SCANCODE_LEFT: {
                                    if(arrowReleased[3] || arrowTurbo){
                                        if(shiftPressed){
                                            if(!gapGetSelectionSize(&buffer)){
                                                // there is no current selection alive
                                                buffer.selection.end = buffer.cursor;
                                            }
                                        } else {
                                            buffer.selection.start = buffer.selection.end;
                                        }

                                        if(controlSeeking){
                                            gapSeekCursorToPreviousCapitalOrSpace(&buffer);
                                            if(buffer.data[UserToGap(buffer.gap, buffer.cursor)] == '\n'){
                                                gapIncreaseCursor(&buffer);
                                            }
                                        } else {
                                            gapDecreaseCursor(&buffer);
                                        }

                                        if(shiftPressed){
                                            buffer.selection.start = buffer.cursor;
                                        }
                                    } else {
                                        arrowTime++;
                                        if (arrowTime >= TURBO_TIME){
                                            arrowTurbo = true;
                                        }
                                    }
                                    arrowReleased[3] = false;
                                }
                                break;

                            case SDL_SCANCODE_RIGHT: {
                                    if(arrowReleased[2] || arrowTurbo){
                                        if(shiftPressed){
                                            if(!gapGetSelectionSize(&buffer)){
                                                // there is no current selection alive
                                                buffer.selection.start = buffer.cursor;
                                            }
                                        } else {
                                            buffer.selection.start = buffer.selection.end;
                                        }

                                        if(controlSeeking){
                                            gapSeekCursorToCapitalOrSpace(&buffer);
                                        } else {
                                            gapIncreaseCursor(&buffer);
                                        }

                                        if(shiftPressed){
                                            buffer.selection.end = buffer.cursor;
                                        }
                                    } else {
                                        arrowTime++;
                                        if (arrowTime >= TURBO_TIME){
                                            arrowTurbo = true;
                                        }
                                    }
                                    arrowReleased[2] = false;
                                }
                                break;

                            case SDL_SCANCODE_DELETE: {
                                    if(backspaceReleased || backspaceTurbo){
                                        gapRemoveCharacterNearAt(&buffer, buffer.cursor);
                                    } else {
                                        backspaceTime++;
                                        if (backspaceTime >= TURBO_TIME){
                                            backspaceTurbo = true;
                                        }
                                    }
                                    backspaceReleased = false;
                                }
                                break;


                            case SDL_SCANCODE_BACKSPACE: {
                                    if(backspaceReleased || backspaceTurbo){
                                        gapRemoveCharacterAt(&buffer, buffer.cursor);
                                        gapDecreaseCursor(&buffer);
                                    } else {
                                        backspaceTime++;
                                        if (backspaceTime >= TURBO_TIME){
                                            backspaceTurbo = true;
                                        }
                                    }
                                    backspaceReleased = false;
                                }
                                break;
                                
                            default: {
                                    char character = event.key.keysym.sym;
                                    if(shiftPressed) {
                                        character = shiftCharactersLUT[character];
                                    }
                                    if(!controlSeeking){
                                        gapInsertCharacterAt(&buffer, character, buffer.cursor);
                                        gapIncreaseCursor(&buffer);
                                    } else {
                                        switch (character) {
                                            case 'v': {
                                                    if(SDL_HasClipboardText()){
                                                        char* clipboard = SDL_GetClipboardText();
                                                        if(clipboard){
                                                            i32 distance = gapInsertNullTerminatedStringAt(&buffer, clipboard, buffer.cursor);
                                                            gapSeekCursor(&buffer, distance);
                                                            SDL_free(clipboard);
                                                        }
                                                    }
                                                }
                                                break;

                                            case 'c': {
                                                    String copy = {};
                                                    if(buffer.selection.end < buffer.gap.start){
                                                        copy = subString(buffer.bufferString, buffer.selection.start, buffer.selection.end);
                                                    } else {
                                                        copy = subString(buffer.bufferString, UserToGap(buffer.gap, buffer.selection.start), UserToGap(buffer.gap, buffer.selection.end));
                                                    }
                                                    i32 status = SDL_SetClipboardText((char*)copy.data);
                                                    if(status){
                                                        printf("SDL_SetClipboardText failed: %s\n", SDL_GetError());
                                                    }
                                                }
                                                break;


                                            default:{
                                                }
                                                break;
                                        }
                                    }
                                    buffer.selection.end = buffer.selection.start;
                                }
                                break;
                        }
                    }
                    break;

                case SDL_QUIT: {
                        done = true;
                    }
                    break;
            }
        }


        v2 uvs[4] = {};

        glViewport(0, 0, windowWidth, windowHeight);

        glClearColor(0.1, 0.1, 0.1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font.textureId);
        
        cursor.y += 2;
        cursor = v3();
        for(int i = 0; i < buffer.cursor; ++i){
            switch(buffer.data[UserToGap(buffer.gap, i)]){
                case 0:{
                    }
                    break;

                case '\t': {
                        cursor.x += FONT_HEIGHT * 2;
                    }
                    break;

                case '\n': {
                        cursor.y += FONT_HEIGHT;
                        cursor.x = 0;
                    }
                    break;

                default: {
                        Glyph glyph = font.glyphs[buffer.data[UserToGap(buffer.gap, i)] - ' '];
                        cursor.x += glyph.xadvance;
                    }
                    break;
            }
        }
        
        cursor.y += 3;
        if(cursor.y <= windowTop){
            i32 distance = windowTop - cursor.y;
            windowTransform *= translate({0, distance, 0});
            windowBottom -= distance;
            windowTop -= distance;
        } else if(cursor.y >= windowBottom){
            i32 distance = cursor.y - windowBottom + FONT_HEIGHT;
            windowTransform *= translate({0, -distance, 0});
            windowBottom += distance;
            windowTop += distance;
        }

        pushQuad(&renderBufferBackground, v3(0, cursor.y, 0), {windowWidth, FONT_HEIGHT + 3}, uvs, v3(0, 0, 0));

        SHADER_SCOPE(shaderUI.programId, {
            shaderSetUniform4m(shaderUI.locations.matrixView, windowView);
            shaderSetUniform4m(shaderUI.locations.matrixTransform, windowTransform);
            flushRenderBuffer(GL_TRIANGLES, &renderBufferBackground);
        });

        fontRenderGapBuffer(&buffer, &renderBuffer, &renderBufferUI, &font);

        if(time < 10){
            pushQuad(&renderBufferUI, cursor, {FONT_HEIGHT / 2, FONT_HEIGHT + 3}, uvs, v3(0, 1, 1));
        }

        SHADER_SCOPE(shader.programId, {
            shaderSetUniform4m(shader.locations.matrixView, windowView);
            shaderSetUniform4m(shader.locations.matrixTransform, windowTransform);
            flushRenderBuffer(GL_TRIANGLES, &renderBuffer);
        });


        SHADER_SCOPE(shaderUI.programId, {
            shaderSetUniform4m(shaderUI.locations.matrixView, windowView);
            shaderSetUniform4m(shaderUI.locations.matrixTransform, windowTransform);
            flushRenderBuffer(GL_TRIANGLES, &renderBufferUI);
        });
        

        SDL_GL_SwapWindow(window);
    }
#endif
    return 0;
}


