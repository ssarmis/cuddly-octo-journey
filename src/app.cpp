#include <SDL2/SDL.h>

#include "gl.h"
#include "gl_utilities.h"
#include "render_buffer.h"
#include "math.h"
#include "shader.h"
#include "font.h"

#include "gap_buffer.h"
#include "window.h"

#include <time.h>

#define TURBO_TIME 1

u32 windowWidth = 1280;
u32 windowHeight = 768;

u32 windowCount = 2;
EditorWindow windows[2] = {
    windowCreate(windowWidth / 2, windowHeight, 0, 0),
    windowCreate(windowWidth / 2, windowHeight, windowWidth / 2, 0)
};

u32 currentWindowIndex = 0;
EditorWindow* currentWindow = &windows[currentWindowIndex];

int main(int argumentCount, char* arguments[]){
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
#endif

    // GapBuffer buffer = {};
    // buffer.data = new u8[GAP_DEFAULT_SIZE];
    // memset(buffer.data, 0, GAP_DEFAULT_SIZE);
    // buffer.size = GAP_DEFAULT_SIZE;
    // buffer.cursor = 0;
    // buffer.gap.start = 0;
    // buffer.gap.end = GAP_DEFAULT_SIZE;

    if(argumentCount > 1){
        currentWindow->buffer = gapReadFile(arguments[1]);
    } else {
        currentWindow->buffer = gapCreateEmpty();
    }

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
    shiftCharactersLUT['.'] = '>';
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
                                    u32 newWidth = event.window.data1;
                                    u32 newHeight = event.window.data2;
                                    // TODO(Sarmis) I have to fix these 
                                    // depending on the new size,
                                    // the already existing offset needs to 
                                    // update


                                    // TODO(Sarmis) after the multiple windows
                                    // were implemented, the resizing 
                                    // doesn't fix the windows dimensions 
                                    // properly
                                    for(int i = 0; i < windowCount; ++i){
                                        r32 wr = windows[i].width / windowWidth;
                                        r32 hr = windows[i].height / windowHeight;

                                        windows[i].width = wr * newWidth;
                                        windows[i].height = hr * newHeight;
                                        windows[i].top = hr * newHeight;

                                        windows[i].bottom = windows[i].top + windows[i].height;
                                    }

                                    SHADER_SCOPE(shader.programId, {
                                        shaderSetUniform4m(shader.locations.matrixPerspective, 
                                                        orthographic(0, newWidth, 0, newHeight));
                                    });

                                    SHADER_SCOPE(shaderUI.programId, {
                                        shaderSetUniform4m(shaderUI.locations.matrixPerspective, 
                                                        orthographic(0, newWidth, 0, newHeight));
                                    });

                                    windowWidth = newWidth;
                                    windowHeight = newHeight;
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
                                        gapSeekCursorINewlinesIfPossible(&currentWindow->buffer, -heightInLines);
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
                                        gapSeekCursorINewlinesIfPossible(&currentWindow->buffer, heightInLines);
                                    }
                                    pageDownReleased = false;
                                }
                                break;

                            case SDL_SCANCODE_HOME: {
                                    if(homeReleased){
                                        if(shiftPressed){
                                            if(!gapGetSelectionSize(&currentWindow->buffer)){
                                                // there is no current selection alive
                                                currentWindow->buffer.selection.end = currentWindow->buffer.cursor;
                                            }
                                        } else {
                                            currentWindow->buffer.selection.start = currentWindow->buffer.selection.end;
                                        }

                                        gapSeekCursorToPreviousNewline(&currentWindow->buffer);

                                        if(currentWindow->buffer.data[UserToGap(currentWindow->buffer.gap, currentWindow->buffer.cursor)] == '\n'){
                                            gapIncreaseCursor(&currentWindow->buffer);
                                        }

                                        if(shiftPressed){
                                            currentWindow->buffer.selection.start = currentWindow->buffer.cursor;
                                        }
                                    }
                                    homeReleased = false;
                                }
                                break;

                            case SDL_SCANCODE_END: {
                                    // TODO(Sarmis) this could end at a weird 
                                    // location at the end of the buffer
                                    // add bound checking
                                    if(endReleased){
                                        if(shiftPressed){
                                            if(!gapGetSelectionSize(&currentWindow->buffer)){
                                                // there is no current selection alive
                                                currentWindow->buffer.selection.start = currentWindow->buffer.cursor;
                                            }
                                        } else {
                                            currentWindow->buffer.selection.start = currentWindow->buffer.selection.end;
                                        }

                                        if(currentWindow->buffer.data[UserToGap(currentWindow->buffer.gap, currentWindow->buffer.cursor)] != '\n'){
                                            gapSeekCursorToNewline(&currentWindow->buffer);
                                        }

                                        if(shiftPressed){
                                            currentWindow->buffer.selection.end = currentWindow->buffer.cursor;
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
                                        if(controlSeeking){
                                            ++currentWindowIndex;
                                            currentWindowIndex %= windowCount;
                                            currentWindow = &windows[currentWindowIndex];
                                        } else {
                                            gapInsertCharacterAt(&currentWindow->buffer, tab, currentWindow->buffer.cursor);
                                            gapIncreaseCursor(&currentWindow->buffer);
                                        }
                                    }
                                    tabReleased = false;
                                }
                                break;

                            case SDL_SCANCODE_RETURN2: case SDL_SCANCODE_RETURN: {
                                    u8 newline = '\n';
                                    if(newlineReleased){
                                        i32 tabs = gapGetAmontOfTabsBeforeCursor(&currentWindow->buffer);

                                        gapInsertCharacterAt(&currentWindow->buffer, newline, currentWindow->buffer.cursor);
                                        gapIncreaseCursor(&currentWindow->buffer);

                                        while(tabs--){
                                            gapInsertCharacterAt(&currentWindow->buffer, '\t', currentWindow->buffer.cursor);
                                            gapIncreaseCursor(&currentWindow->buffer);
                                        }
                                    }
                                    newlineReleased = false;
                                }
                                break;

                            case SDL_SCANCODE_UP: {
                                    // TODO(Sarmis) this doesn't really work properly
                                    // did this quick to just have a basic thing ready
                                    // for this thing...
                                    if(arrowReleased[1] || arrowTurbo){
                                        i32 distance = gapGetDistanceFromPreviousNewline(&currentWindow->buffer);

                                        if(shiftPressed){
                                            if(!gapGetSelectionSize(&currentWindow->buffer)){
                                                // there is no current selection alive
                                                currentWindow->buffer.selection.end = currentWindow->buffer.cursor;
                                            }
                                        } else {
                                            currentWindow->buffer.selection.start = currentWindow->buffer.selection.end;
                                        }

                                        if(controlSeeking){
                                            gapSeekCursorINewlinesIfPossible(&currentWindow->buffer, -10);
                                        } else {
                                            gapSeekCursorToPreviousNewline(&currentWindow->buffer);
                                            if(currentWindow->buffer.cursor - 1 >= 0){
                                                i32 distanceOnPreviousLine = gapGetDistanceFromPreviousNewline(&currentWindow->buffer);
                                                distanceOnPreviousLine -= distance;
                                                if(distanceOnPreviousLine >= 0){
                                                    gapSeekCursor(&currentWindow->buffer, -distanceOnPreviousLine);
                                                }
                                            }
                                        }

                                        if(shiftPressed){
                                            currentWindow->buffer.selection.start = currentWindow->buffer.cursor;
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
                                        i32 distance = gapGetDistanceFromPreviousNewline(&currentWindow->buffer);

                                        if(shiftPressed){
                                            if(!gapGetSelectionSize(&currentWindow->buffer)){
                                                // there is no current selection alive
                                                currentWindow->buffer.selection.start = currentWindow->buffer.cursor;
                                            }
                                        } else {
                                            currentWindow->buffer.selection.start = currentWindow->buffer.selection.end;
                                        }

                                        if(controlSeeking){
                                            gapSeekCursorINewlinesIfPossible(&currentWindow->buffer, 10);
                                        } else {
                                            gapSeekCursorToNewline(&currentWindow->buffer);

                                            if(currentWindow->buffer.cursor + 1 < currentWindow->buffer.size){
                                                gapIncreaseCursor(&currentWindow->buffer);
                                                i32 lineLength = gapGetDistanceToNewline(&currentWindow->buffer);
                                                if(distance <= lineLength){
                                                    gapSeekCursor(&currentWindow->buffer, distance - 1);
                                                } else {
                                                    gapSeekCursor(&currentWindow->buffer, lineLength);
                                                }
                                            }
                                        }

                                        if(shiftPressed){
                                            currentWindow->buffer.selection.end = currentWindow->buffer.cursor;
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
                                            if(!gapGetSelectionSize(&currentWindow->buffer)){
                                                // there is no current selection alive
                                                currentWindow->buffer.selection.end = currentWindow->buffer.cursor;
                                            }
                                        } else {
                                            currentWindow->buffer.selection.start = currentWindow->buffer.selection.end;
                                        }

                                        if(controlSeeking){
                                            gapSeekCursorToPreviousCapitalOrSpace(&currentWindow->buffer);
                                            if(currentWindow->buffer.data[UserToGap(currentWindow->buffer.gap, currentWindow->buffer.cursor)] == '\n'){
                                                gapIncreaseCursor(&currentWindow->buffer);
                                            }
                                        } else {
                                            gapDecreaseCursor(&currentWindow->buffer);
                                        }

                                        if(shiftPressed){
                                            currentWindow->buffer.selection.start = currentWindow->buffer.cursor;
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
                                            if(!gapGetSelectionSize(&currentWindow->buffer)){
                                                // there is no current selection alive
                                                currentWindow->buffer.selection.start = currentWindow->buffer.cursor;
                                            }
                                        } else {
                                            currentWindow->buffer.selection.start = currentWindow->buffer.selection.end;
                                        }

                                        if(controlSeeking){
                                            gapSeekCursorToCapitalOrSpace(&currentWindow->buffer);
                                        } else {
                                            gapIncreaseCursor(&currentWindow->buffer);
                                        }

                                        if(shiftPressed){
                                            currentWindow->buffer.selection.end = currentWindow->buffer.cursor;
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
                                        if(gapGetSelectionSize(&currentWindow->buffer)){
                                            gapRemoveCharactersInRange(&currentWindow->buffer, currentWindow->buffer.selection.start, currentWindow->buffer.selection.end);
                                            gapSeekCursor(&currentWindow->buffer, -(currentWindow->buffer.selection.end - currentWindow->buffer.selection.start));
                                        } else {
                                            gapRemoveCharacterNearAt(&currentWindow->buffer, currentWindow->buffer.cursor);
                                        }
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
                                    // TODO(Sarmis) this results in segfault, needs bound checking
                                    if(backspaceReleased || backspaceTurbo){
                                        if(gapGetSelectionSize(&currentWindow->buffer)){
                                            gapRemoveCharactersInRange(&currentWindow->buffer, currentWindow->buffer.selection.start, currentWindow->buffer.selection.end);
                                            gapSeekCursor(&currentWindow->buffer, -(currentWindow->buffer.selection.end - currentWindow->buffer.selection.start));
                                        } else {
                                            if(!controlSeeking){
                                                gapRemoveCharacterAt(&currentWindow->buffer, currentWindow->buffer.cursor);
                                                gapDecreaseCursor(&currentWindow->buffer);
                                            } else {
                                                u32 end = currentWindow->buffer.cursor;
                                                gapSeekCursorToPreviousCapitalOrSpace(&currentWindow->buffer);
                                                u32 start = currentWindow->buffer.cursor + 1;
                                                gapRemoveCharactersInRange(&currentWindow->buffer, start, end);
                                            }
                                        }
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
                                        gapInsertCharacterAt(&currentWindow->buffer, character, currentWindow->buffer.cursor);
                                        gapIncreaseCursor(&currentWindow->buffer);
                                    } else {
                                        switch (character) {
                                            case 's': {
                                                    if(currentWindow->buffer.dirty){
                                                    }

                                                    if(!currentWindow->buffer.file){
                                                        // file is not on disk
                                                        // this is the first save ever
                                                        // for this file...
                                                        gapWriteFile(&currentWindow->buffer, "lol.tmp");
                                                    }
                                                }
                                                break;

                                            case 'v': {
                                                    if(SDL_HasClipboardText()){
                                                        char* clipboard = SDL_GetClipboardText();
                                                        if(clipboard){
                                                            i32 distance = gapInsertNullTerminatedStringAt(&currentWindow->buffer, clipboard, currentWindow->buffer.cursor);
                                                            gapSeekCursor(&currentWindow->buffer, distance);
                                                            SDL_free(clipboard);
                                                        }
                                                    }
                                                }
                                                break;

                                            case 'c': {
                                                    String copy = {};
                                                    // TODO(Sarmis) solve case in which 
                                                    // the gap in in the selection
                                                    if(currentWindow->buffer.selection.end < currentWindow->buffer.gap.start){
                                                        copy = subString(currentWindow->buffer.bufferString, currentWindow->buffer.selection.start, currentWindow->buffer.selection.end);
                                                    } else {
                                                        copy = subString(currentWindow->buffer.bufferString, UserToGap(currentWindow->buffer.gap, currentWindow->buffer.selection.start), UserToGap(currentWindow->buffer.gap, currentWindow->buffer.selection.end));
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
                                    currentWindow->buffer.selection.end = currentWindow->buffer.selection.start;
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

        glClearColor(DEFAULT_COLOR_BACKGROUND.x, DEFAULT_COLOR_BACKGROUND.y, DEFAULT_COLOR_BACKGROUND.z, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, font.textureId);
        
        for(int i = 0; i < windowCount; ++i){
            EditorWindow* window = &windows[i];
            window->cursor = v3(window->left, window->top, 0);
            for(int ii = 0; ii < window->buffer.cursor; ++ii){
                switch(window->buffer.data[UserToGap(window->buffer.gap, ii)]){
                    case 0:{
                        }
                        break;

                    case '\t': {
                            window->cursor.x += FONT_HEIGHT * 2;
                        }
                        break;

                    case '\n': {
                            window->cursor.y += FONT_HEIGHT;
                            window->cursor.x = window->left;
                        }
                        break;

                    default: {
                            Glyph glyph = font.glyphs[window->buffer.data[UserToGap(window->buffer.gap, ii)] - ' '];
                            window->cursor.x += glyph.xadvance;
                        }
                        break;
                }
            }

            window->cursor.y += 3;
            if(window->cursor.y <= window->scrollTop){
                i32 distance = window->scrollTop - window->cursor.y;
                window->transform *= translate({0, distance, 0});
                window->scrollBottom -= distance;
                window->scrollTop -= distance;
            } else if(window->cursor.y >= window->scrollBottom){
                i32 distance = window->cursor.y - window->scrollBottom + FONT_HEIGHT;
                window->transform *= translate({0, -distance, 0});
                window->scrollBottom += distance;
                window->scrollTop += distance;
            }

            // current editing line
            pushQuad(&renderBufferBackground, v3(window->left, window->cursor.y, 0), {window->width, FONT_HEIGHT + 3}, uvs, v3(0, 0, 0));

            SHADER_SCOPE(shaderUI.programId, {
                shaderSetUniform4m(shaderUI.locations.matrixView, window->view);
                shaderSetUniform4m(shaderUI.locations.matrixTransform, window->transform);
                shaderSetUniform32u(shaderUI.locations.boundsLeft, window->left);
                shaderSetUniform32u(shaderUI.locations.boundsRight, window->left + window->width);
                shaderSetUniform32u(shaderUI.locations.boundsTop, window->top);
                shaderSetUniform32u(shaderUI.locations.boundsBottom, window->top + window->height);
                flushRenderBuffer(GL_TRIANGLES, &renderBufferBackground);
            });

            fontRenderGapBuffer({window->left, window->top}, &window->buffer, &renderBuffer, &renderBufferUI, &font);

            if(i == currentWindowIndex){
                if(time < 10){
                    pushQuad(&renderBufferUI, window->cursor, {FONT_HEIGHT / 2, FONT_HEIGHT + 3}, uvs, v3(0, 1, 1));
                }
            }

            SHADER_SCOPE(shader.programId, {
                shaderSetUniform4m(shader.locations.matrixView, window->view);
                shaderSetUniform4m(shader.locations.matrixTransform, window->transform);
                shaderSetUniform32u(shader.locations.boundsLeft, window->left);
                shaderSetUniform32u(shader.locations.boundsRight, window->left + window->width);
                shaderSetUniform32u(shader.locations.boundsTop, window->top);
                shaderSetUniform32u(shader.locations.boundsBottom, window->top + window->height);
                flushRenderBuffer(GL_TRIANGLES, &renderBuffer);
            });

            SHADER_SCOPE(shaderUI.programId, {
                shaderSetUniform4m(shaderUI.locations.matrixView, window->view);
                shaderSetUniform4m(shaderUI.locations.matrixTransform, window->transform);
                shaderSetUniform32u(shaderUI.locations.boundsLeft, window->left);
                shaderSetUniform32u(shaderUI.locations.boundsRight, window->left + window->width);
                shaderSetUniform32u(shaderUI.locations.boundsTop, window->top);
                shaderSetUniform32u(shaderUI.locations.boundsBottom, window->top + window->height);
                flushRenderBuffer(GL_TRIANGLES, &renderBufferUI);
            });
        }

        SDL_GL_SwapWindow(window);
    }
#endif
    return 0;
}


