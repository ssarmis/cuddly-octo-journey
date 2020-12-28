#pragma once

#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"

#include "general.h"
#include "gl.h"

#include <stdio.h>

#include "render_buffer.h"
#include "gap_buffer.h"

#include "keywords.h"

#define FONT_HEIGHT 16

#define Glyph stbtt_bakedchar
#define GlyphSize stbtt_aligned_quad

struct FontGL {
    Glyph glyphs[96];
    GLuint textureId;
};

void fontRenderGapBufferNoHighlights(v2 position, GapBuffer* buffer, RenderBuffer* renderBuffer, RenderBuffer* renderBufferUI, FontGL* font, r32 upperLine=0, r32 bottomLine=0, v3 defaultColor=DEFAULT_COLOR_TEXT){
    v2 cursor = position;
    cursor += v2(0, FONT_HEIGHT);

    v2 selectionCursor = position;
    v3 color = defaultColor;
    u32 size = 0;

    glBindBuffer(GL_ARRAY_BUFFER, renderBuffer->vertexBufferId);
    Vertex* vertices = NULL;

    vertices = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    for(int i = 0; i < buffer->size; ++i){
        if(i >= buffer->gap.start && i < buffer->gap.end){
            continue;
        }

        if(!buffer->data[i]){
            continue;
        }
        
        char character = buffer->data[i];

        if(character == '\n'){
            cursor.x = position.x;
            cursor.y += FONT_HEIGHT;
        } else if(character == '\t'){
            cursor.x += FONT_HEIGHT * 2;
        } else {
            i32 glyphIndex = character - ' ';
            Glyph glyph = font->glyphs[glyphIndex];

            if(cursor.y < upperLine){
                continue;
            } else if(cursor.y > bottomLine){
                break;
            }

            v2 uvs[] = {
                v2(glyph.x0 / 512.0, glyph.y1 / 512.0),
                v2(glyph.x1 / 512.0, glyph.y1 / 512.0),
                v2(glyph.x0 / 512.0, glyph.y0 / 512.0),
                v2(glyph.x1 / 512.0, glyph.y0 / 512.0),
            };

            r32 w = (glyph.x1 - glyph.x0);
            r32 h = (glyph.y1 - glyph.y0);

            v3 position = v3(cursor.x + glyph.xoff, cursor.y + glyph.yoff, 0);

            *vertices++ = Vertex(position,               uvs[2], color);
            *vertices++ = Vertex(position + v3(w, 0, 0), uvs[3], color);
            *vertices++ = Vertex(position + v3(0, h, 0), uvs[0], color);
            *vertices++ = Vertex(position + v3(w, h, 0), uvs[1], color);

            renderBuffer->totalUsedVertices += 4;
            renderBuffer->totalUsedIndices += 6;

            cursor.x += (glyph.xadvance);
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    v2 uvs[4] = {};

    i32 selectionSize = gapGetSelectionSize(buffer);
    if(selectionSize){
        // TODO(Sarmis) not proud of this...will change...
        i32 selectionBeggining;
        i32 selectionEnding;
        if(selectionSize > 0){
            selectionBeggining = UserToGap(buffer->gap, buffer->selection.start);
            selectionEnding = UserToGap(buffer->gap, buffer->selection.end);
        } else {
            selectionBeggining = UserToGap(buffer->gap, buffer->selection.end);
            selectionEnding = UserToGap(buffer->gap, buffer->selection.start);
        }
        
        for(int i = 0; i < buffer->size; ++i){
            if(i >= buffer->gap.start && i < buffer->gap.end){
                continue;
            }

            char character = buffer->data[i];
        
            if(character == '\n'){
                selectionCursor.x = position.x;
                selectionCursor.y += FONT_HEIGHT;
            } else if(character == '\t'){
                selectionCursor.x += FONT_HEIGHT * 2;
            } else {
                i32 glyphIndex = character - ' ';
                Glyph glyph = font->glyphs[glyphIndex];

                if(i >= selectionBeggining &&
                    i < selectionEnding){
                    
                    // TODO(Sarmis) this would be nice to not need to be computed everytime
                    // just store it in the glyph, change the stb header or just wrap the 
                    // baked glyph
                    r32 glyphWidth = glyph.x1 - glyph.x0;
                    r32 w = glyphWidth + glyph.xadvance - glyphWidth;
                    r32 h = FONT_HEIGHT;

                    pushQuad(renderBufferUI, v3(selectionCursor.x, selectionCursor.y + 3, 0), 
                            v2(w, h), uvs, SELECTION_COLOR_TEXT);
                }

                selectionCursor.x += (glyph.xadvance);
            }
        }
    }
}

void fontRenderGapBuffer(v2 position, GapBuffer* buffer, RenderBuffer* renderBuffer, RenderBuffer* renderBufferUI, FontGL* font, r32 upperLine=0, r32 bottomLine=0, v3 defaultColor=DEFAULT_COLOR_TEXT){
    v2 cursor = position;
    cursor += v2(0, FONT_HEIGHT);

    v2 selectionCursor = position;
    v3 color = defaultColor;
    u32 size = 0;

    glBindBuffer(GL_ARRAY_BUFFER, renderBuffer->vertexBufferId);
    Vertex* vertices = NULL;

    vertices = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    bool isString = false;
    bool isComment = false;

    for(int i = 0; i < buffer->size; ++i){
        if(i >= buffer->gap.start && i < buffer->gap.end){
            continue;
        }

        if(!buffer->data[i]){
            continue;
        }
        
        char character = buffer->data[i];
        char nextCharacter = 0;
        if(i + 1 <= buffer->size - 1){
            nextCharacter = buffer->data[i + 1];
        }

        if(character == '\n'){
            cursor.x = position.x;
            cursor.y += FONT_HEIGHT;
            isComment = false;
        } else if(character == '\t'){
            cursor.x += FONT_HEIGHT * 2;
        } else {
            i32 glyphIndex = character - ' ';
            Glyph glyph = font->glyphs[glyphIndex];

            if(cursor.y < upperLine){
                continue;
            } else if(cursor.y > bottomLine){
                break;
            }

            if(nextCharacter){
                if(character == '/' && nextCharacter == '/'){
                    isComment = true;
                }
            }

            if(!isComment){
                if(!size){
                    // NOTE(Sarmis) if you ever though you wrote bad code
                    // you never saw this approach/function here
                    keywordPeek(&buffer->data[i], &color, &size);
                }
            }

            if(size){
                --size;
                if(!size){
                    color = defaultColor;
                }
            }

            v2 uvs[] = {
                v2(glyph.x0 / 512.0, glyph.y1 / 512.0),
                v2(glyph.x1 / 512.0, glyph.y1 / 512.0),
                v2(glyph.x0 / 512.0, glyph.y0 / 512.0),
                v2(glyph.x1 / 512.0, glyph.y0 / 512.0),
            };

            r32 w = (glyph.x1 - glyph.x0);
            r32 h = (glyph.y1 - glyph.y0);

            if(!isString){
                if(character == '"'){
                    isString = true;
                }
            } else {
                if(character == '"'){
                    isString = false;
                }
            }

            if(isComment){
                color = DEFAULT_COLOR_COMMENT;
            } else if(isString) {
                color = STRING_COLOR_TEXT;
            }

            v3 position = v3(cursor.x + glyph.xoff, cursor.y + glyph.yoff, 0);

            *vertices++ = Vertex(position,               uvs[2], color);
            *vertices++ = Vertex(position + v3(w, 0, 0), uvs[3], color);
            *vertices++ = Vertex(position + v3(0, h, 0), uvs[0], color);
            *vertices++ = Vertex(position + v3(w, h, 0), uvs[1], color);

            renderBuffer->totalUsedVertices += 4;
            renderBuffer->totalUsedIndices += 6;

            cursor.x += (glyph.xadvance);

            if(!isString && !size){
                color = defaultColor;
            }
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    v2 uvs[4] = {};
    
    i32 selectionSize = gapGetSelectionSize(buffer);
    if(selectionSize){
        // TODO(Sarmis) not proud of this...will change...
        i32 selectionBeggining;
        i32 selectionEnding;
        if(selectionSize > 0){
            selectionBeggining = UserToGap(buffer->gap, buffer->selection.start);
            selectionEnding = UserToGap(buffer->gap, buffer->selection.end);
        } else {
            selectionBeggining = UserToGap(buffer->gap, buffer->selection.end);
            selectionEnding = UserToGap(buffer->gap, buffer->selection.start);
        }
        
        for(int i = 0; i < buffer->size; ++i){
            if(i >= buffer->gap.start && i < buffer->gap.end){
                continue;
            }

            char character = buffer->data[i];

            if(!character){
                continue;
            }
        
            if(character == '\n'){
                selectionCursor.x = position.x;
                selectionCursor.y += FONT_HEIGHT;
            } else if(character == '\t'){
                selectionCursor.x += FONT_HEIGHT * 2;
            } else {
                i32 glyphIndex = character - ' ';
                Glyph glyph = font->glyphs[glyphIndex];

                if(i >= selectionBeggining &&
                    i < selectionEnding){

                    // TODO(Sarmis) this would be nice to not need to be computed everytime
                    // just store it in the glyph, change the stb header or just wrap the 
                    // baked glyph
                    r32 glyphWidth = glyph.x1 - glyph.x0;
                    r32 w = glyphWidth + glyph.xadvance - glyphWidth;
                    r32 h = FONT_HEIGHT;

                    pushQuad(renderBufferUI, v3(selectionCursor.x, selectionCursor.y + 3, 0), 
                            v2(w, h), uvs, SELECTION_COLOR_TEXT);
                }

                selectionCursor.x += (glyph.xadvance);
            }
        }
    }
}

// TODO(Sarmis) this function needs to take a starting position for the cursor
void fontRender(u8* string, u32 size, v2 position, RenderBuffer* renderBuffer, FontGL* font, v3 color={1, 1, 1}){
    v2 cursor = position;
    while(size--){
        char c = *string - ' ';
        Glyph glyph = font->glyphs[c];
        if(*string == '\n'){
            cursor.x = 0;    
            cursor.y += 32;
        } else {
            v2 uvs[] = {
                v2(glyph.x0 / 512.0, glyph.y1 / 512.0),
                v2(glyph.x1 / 512.0, glyph.y1 / 512.0),
                v2(glyph.x0 / 512.0, glyph.y0 / 512.0),
                v2(glyph.x1 / 512.0, glyph.y0 / 512.0),
            };

            r32 w = (glyph.x1 - glyph.x0);
            r32 h = (glyph.y1 - glyph.y0);

            pushQuad(renderBuffer, v3(cursor.x + (glyph.xoff), cursor.y + glyph.yoff, 0), v2(w, h), uvs, color);
            cursor.x += (glyph.xadvance);
        }
        ++string;
    }
}

void fontRender(String text, v2 position, RenderBuffer* renderBuffer, FontGL* font){
    fontRender(text.data, text.size, position, renderBuffer, font);
}

FontGL createFont(){
    FontGL result = {};

    u8* bitmap = new u8[512 * 512];
    u8* fontFileData = new u8[1 << 20];
    fread(fontFileData, 1, 1 << 20, fopen("LiberationMono-Regular.ttf", "rb"));
    stbtt_BakeFontBitmap(fontFileData, 0, FONT_HEIGHT, bitmap, 512, 512, 32, 96, result.glyphs); // no guarantee this fits!
    
    
    glGenTextures(1, &result.textureId);
    glBindTexture(GL_TEXTURE_2D, result.textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

    delete[] fontFileData;
    delete[] bitmap;

    return result;
}
