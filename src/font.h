#pragma once

#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"

#include "general.h"
#include "gl.h"

#include <stdio.h>

#include "render_buffer.h"

#include "keywords.h"

#define FONT_HEIGHT 22

#define Glyph stbtt_bakedchar
#define GlyphSize stbtt_aligned_quad
        //  stbtt_aligned_quad q;
        //  stbtt_GetBakedQuad(cdata, 512,512, *text-32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9


struct FontGL {
    Glyph glyphs[96];
    GLuint textureId;
};

#include "gap_buffer.h"
#include "linked_lines.h"

void fontRenderLinkedLines(LinkedLine* firstLine, RenderBuffer* renderBuffer, FontGL* font){
    LinkedLine* iterator = firstLine;
    u32 lineCount = 1;

    v2 cursor = v2(0, 0);

    while(iterator){
        cursor.y = lineCount++ * FONT_HEIGHT;
        cursor.x = 0;

        for(int i = 0; i < iterator->data.currentAmount; ++i){
            u8 originalCharacter = iterator->data.array[i];
            char c = originalCharacter - ' ';
            Glyph glyph = font->glyphs[c];

            if(originalCharacter == '\t'){
                cursor.x += FONT_HEIGHT * 2;    
            } else {
                v2 uvs[] = {
                    v2(glyph.x0 / 512.0, glyph.y1 / 512.0),
                    v2(glyph.x1 / 512.0, glyph.y1 / 512.0),
                    v2(glyph.x0 / 512.0, glyph.y0 / 512.0),
                    v2(glyph.x1 / 512.0, glyph.y0 / 512.0),
                };

                r32 w = (glyph.x1 - glyph.x0);
                r32 h = (glyph.y1 - glyph.y0);

                r32 yo = 0;

                pushQuad(renderBuffer, v3(cursor.x + glyph.xoff, cursor.y + glyph.yoff, 0), v2(w, h), uvs);
                cursor.x += (glyph.xadvance);
            }
        }
        iterator = iterator->next;
    }
}

void fontRenderGapBuffer(GapBuffer* buffer, RenderBuffer* renderBuffer, RenderBuffer* renderBufferUI, FontGL* font){
    v2 cursor = v2(0, FONT_HEIGHT);
    v2 selectionCursor = v2(0, 0);
    v3 color = {1, 1, 1};
    u32 size = 0;

    glBindBuffer(GL_ARRAY_BUFFER, renderBuffer->vertexBufferId);
    Vertex* vertices = NULL;

    vertices = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    bool isString = false;

    for(int i = 0; i < buffer->size; ++i){
        if(i >= buffer->gap.start && i < buffer->gap.end){
            continue;
        }

        char character = buffer->data[i];
        if(!size){
            keywordPeek(&buffer->data[i], &color, &size);
        }

        if(size){
            --size;
            if(!size){
                color = {1, 1, 1};
            }
        }

        if(!buffer->data[i]){
            continue;
        }

        i32 glyphIndex = character - ' ';
        Glyph glyph = font->glyphs[glyphIndex];
        
        if(character == '\n'){
            cursor.x = 0;    
            cursor.y += FONT_HEIGHT;

            selectionCursor.x = 0;
            selectionCursor.y += FONT_HEIGHT;
        } else if(character == '\t'){
            cursor.x += FONT_HEIGHT * 2;
            selectionCursor.x += FONT_HEIGHT * 2;
        } else {

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

            if(isString) {
                color = {0.5, 0.1, 0.1};
            }

            // if(i >= buffer->selection.start && i < buffer->selection.end){
            //     pushQuad(renderBufferUI, v3(selectionCursor.x, selectionCursor.y + 3, 0), 
            //             v2(FONT_HEIGHT / 2, FONT_HEIGHT + 3), uvs, {1, 0, 0});
            // }


            v3 position = v3(cursor.x + glyph.xoff, cursor.y + glyph.yoff, 0);

            *vertices++ = Vertex(position,               uvs[2], color);
            *vertices++ = Vertex(position + v3(w, 0, 0), uvs[3], color);
            *vertices++ = Vertex(position + v3(0, h, 0), uvs[0], color);
            *vertices++ = Vertex(position + v3(w, h, 0), uvs[1], color);

            renderBuffer->totalUsedVertices += 4;
            renderBuffer->totalUsedIndices += 6;

            cursor.x += (glyph.xadvance);
            selectionCursor.x += FONT_HEIGHT / 2;

            if(!isString && !size){
                color = {1, 1, 1};
            }
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void fontRender(u8* string, u32 size, RenderBuffer* renderBuffer, FontGL* font){
    v2 cursor = {};
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

            r32 yo = 0;

            if(glyph.yoff < 0){
                yo = h + glyph.yoff;
            }

            pushQuad(renderBuffer, v3(cursor.x + (glyph.xoff), cursor.y - yo, 0), v2(w, h), uvs);
            cursor.x += (glyph.xadvance);
        }
        ++string;
    }
}

void fontRender(String text, RenderBuffer* renderBuffer, FontGL* font){
    fontRender(text.data, text.size, renderBuffer, font);
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
