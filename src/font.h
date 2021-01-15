#pragma once

#include "stb_truetype.h"

#include "general.h"
#include "gl.h"

#include <stdio.h>

#include "render_buffer.h"
#include "gap_buffer.h"

#include "keywords.h"

#define FONT_HEIGHT 24

#define Glyph stbtt_bakedchar
#define GlyphSize stbtt_aligned_quad

struct FontGL {
    Glyph glyphs[96];
    GLuint textureId;
};

static void visualsRenderSelection(v2 position, Selection selection, GapBuffer* buffer, RenderBuffer* renderBuffer, RenderBuffer* renderBufferUI, FontGL* font, v4 color){
    v2 uvs[4] = {};
	v2 selectionCursor = position;
    i32 selectionBeggining = selection.start;
    i32 selectionEnding = selection.end;
    u32 bufferSize = gapGetAbstractSize(buffer);

	for(int i = 0; i < bufferSize; ++i){
        char character = gapCharacterAtIndex(buffer, i);
   
        if(i > selectionEnding){
            break;
        }

        switch(character){
            case '\n': {
                    selectionCursor.x = position.x;
                    selectionCursor.y += FONT_HEIGHT;
                }
                break;

            case '\t': {
                    if(i >= selectionBeggining &&
                        i < selectionEnding){

                        r32 w = FONT_HEIGHT * 2;
                        r32 h = FONT_HEIGHT + 3;
                        pushQuad(renderBufferUI, v3(selectionCursor.x, selectionCursor.y + 3, 0), 
                                v2(w, h), uvs, color);
                    }

                    selectionCursor.x += FONT_HEIGHT * 2;
                }
                break;
            
            default: {
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
                                v2(w, h), uvs, color);
                    }

                    selectionCursor.x += (glyph.xadvance);
                }
                break;
        }
    }
}

static void visualsRenderMarks(v2 position, GapBuffer* buffer, RenderBuffer* renderBuffer, RenderBuffer* renderBufferUI, FontGL* font){
    v2 uvs[4] = {};

    i32 spaceCount = 0;

    Buffer<u32> markedLines = {};
    i32 currentLine = 0;
	bufferAppend<u32>(&markedLines, (u32)0);

    bool passedNewline = false;
    bool passedTab = false;

    u32 bufferSize = gapGetAbstractSize(buffer);

	for(int i = 0; i < bufferSize; ++i){
        char character = gapCharacterAtIndex(buffer, i);

        switch(character){
            case '\n': {
                    spaceCount = 0;
                    if(!markedLines[currentLine]){
                        if(currentLine - 1 >= 0){
                            markedLines[currentLine] = markedLines[currentLine - 1];
                        }
                    }
                    ++currentLine; 
                    bufferAppend<u32>(&markedLines, (u32)0);

                    passedNewline = true;
                }
                break;

            case '\t': {
                    if(passedNewline || passedTab){
                        ++markedLines[currentLine];
                    }

                    passedNewline = false;
                    passedTab = true;
                }
                break;
            
            default: {
                    if(character == ' '){
                        ++spaceCount;
                    } else {
                        spaceCount = 0;
                    }

                    if(!(spaceCount % 4) && spaceCount){
                        spaceCount = 0;
                        ++markedLines[currentLine];
                    }

                    passedNewline = false;
                    passedTab = false;
                }
                break;
        }
    }

    v2 size = v2(1, FONT_HEIGHT);
    for(int i = 0; i < markedLines.currentAmount; ++i){
        u32 marks = markedLines[i];
        for(int ii = 0; ii < marks; ++ii){
            v3 p = v3(position.x + FONT_HEIGHT * 2 * ii, i * FONT_HEIGHT, 0);
            pushQuad(renderBufferUI, p, 
                    size, uvs, v4(1, 1, 1, 0.2));
        }
    }

    bufferClean<u32>(&markedLines);
}

static void fontRenderGapBufferNoHighlights(v2 position, GapBuffer* buffer, RenderBuffer* renderBuffer, RenderBuffer* renderBufferUI, FontGL* font, r32 upperLine=0, r32 bottomLine=0, v4 defaultColor=DEFAULT_COLOR_TEXT){
    v2 cursor = position;
	v2 selectionCursor = position;
    cursor += v2(0, FONT_HEIGHT);

    v4 color = defaultColor;
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

        selectionBeggining = gapUserToGap (buffer->gap, buffer->selection.start);
        selectionEnding = gapUserToGap (buffer->gap, buffer->selection.end);
        
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

static void fontRenderGapBuffer(v2 position, GapBuffer* buffer, RenderBuffer* renderBuffer, RenderBuffer* renderBufferUI, FontGL* font, r32 upperLine=0, r32 bottomLine=0, v4 defaultColor=DEFAULT_COLOR_TEXT){
    v2 cursor = position;
    cursor += v2(0, FONT_HEIGHT);

    v2 selectionCursor = position;
    v4 color = defaultColor;
    u32 size = 0;

    glBindBuffer(GL_ARRAY_BUFFER, renderBuffer->vertexBufferId);
    Vertex* vertices = NULL;

    vertices = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    bool isString = false;
    bool isComment = false;
    bool multilineComment = false;
    bool unsetString = false;

    i32 bufferSize = gapGetAbstractSize(buffer);

    for(int i = 0; i < bufferSize; ++i){
        char character = gapCharacterAtIndex(buffer, i);
        char nextCharacter = 0;
        char previousCharacter = 0;
        if(i + 1 <= bufferSize - 1){
            nextCharacter = gapCharacterAtIndex(buffer, i + 1);
        }

        if(i - 1 >= 0){
            previousCharacter = gapCharacterAtIndex(buffer, i - 1);
        }

        if(character == '\n'){
            cursor.x = position.x;
            cursor.y += FONT_HEIGHT;
			 if(!multilineComment){
	            isComment = false;
            }
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
                } else if(character == '/' && nextCharacter == '*'){
					 multilineComment = true;
                    isComment = true;
                }
	        }

            if(!isComment && !isAlphabeticalCharacter(previousCharacter)){
                if(!size){
                    // TODO(Sarmis) rendering should be based on tokens
                    // NOTE(Sarmis) if you ever though you wrote bad code
                    // you never saw this approach/function here
                    keywordPeek(buffer, i, &color, &size);
                    if(size){
                        if(isAlphabeticalCharacter(gapCharacterAtIndex(buffer, i + size))){
                            size = 0;
                        }
                    }
                }
            }

            if(size){
                --size;
            } else {
                color = defaultColor;
            }

            v2 uvs[] = {
                v2(glyph.x0 / 512.0, glyph.y1 / 512.0),
                v2(glyph.x1 / 512.0, glyph.y1 / 512.0),
                v2(glyph.x0 / 512.0, glyph.y0 / 512.0),
                v2(glyph.x1 / 512.0, glyph.y0 / 512.0),
            };

            r32 w = (glyph.x1 - glyph.x0);
            r32 h = (glyph.y1 - glyph.y0);

			 if(character == '"' && isString){
               unsetString = true;
            } else if(!isString && character == '"'){
		        isString = true;
            }

            if(isComment){
	            color = DEFAULT_COLOR_COMMENT;
            } else if(isString) {
   	         color = STRING_COLOR_TEXT;
            }

 			if(unsetString){
			   unsetString = false;
              isString = false;
	        }

            if(previousCharacter == '*' && character == '/' && multilineComment){
				multilineComment = false;
               isComment = false;
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
}

// TODO(Sarmis) this function needs to take a starting position for the cursor
static void fontRender(u8* string, u32 size, v2 position, RenderBuffer* renderBuffer, FontGL* font, v4 color={1, 1, 1, 1}){
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

static void fontRender(String text, v2 position, RenderBuffer* renderBuffer, FontGL* font){
    fontRender(text.data, text.size, position, renderBuffer, font);
}

static FontGL createFont(char* filename){
    FontGL result = {};

    u8* bitmap = new u8[512 * 512];
    u8* fontFileData = new u8[1 << 20];
    fread(fontFileData, 1, 1 << 20, fopen(filename, "rb"));
    stbtt_BakeFontBitmap(fontFileData, 0, FONT_HEIGHT, bitmap, 512, 512, 32, 96, result.glyphs); // no guarantee this fits!
    
    
    glGenTextures(1, &result.textureId);
    glBindTexture(GL_TEXTURE_2D, result.textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

    delete[] fontFileData;
    delete[] bitmap;

    return result;
}
       