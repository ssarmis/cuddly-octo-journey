#pragma once

#include "general.h"
#include "gl.h"
#include "vertex.h"

#define VERTEX_POSITION_ATTRIBUTE           0
#define VERTEX_UV_ATTRIBUTE                 1
#define VERTEX_COLOR_ATTRIBUTE              2

// NOTE(Sarmis) no more unbinding since its not needed
#define VERTEX_ARRAY_BUFFER_SCOPE(id, scope){\
        glBindVertexArray((id));\
        scope\
        glBindVertexArray(0);\
} 

#define VERTEX_BUFFER_SCOPE(id, scope, type){\
    glBindBuffer((type), (id));\
    scope\
} 

#define TEXTURE_SCOPE(id, scope, type){\
    glBindTexture((type), (id));\
    scope\
}

// NOTE(Sarmis) by default I want all attributes to be enabled
#define ADD_VERTEX_ATTRIB(index, atomCount, type, offset){\
    glVertexAttribPointer((index), (atomCount), (type), GL_FALSE, sizeof(Vertex), (void*)(offset));\
    glEnableVertexAttribArray((index));\
}

#define ADD_VERTEX_ATTRIB_INT(index, atomCount, type, offset){\
    glVertexAttribIPointer((index), (atomCount), (type), sizeof(Vertex), (void*)(offset));\
    glEnableVertexAttribArray((index));\
}

GLint createVertexBuffer(int size){
    GLuint result;
    glGenBuffers(1, &result);
    VERTEX_BUFFER_SCOPE(result, {
        glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
        // TODO(Sarmis) move this out of this function to make this a general purpose function
        ADD_VERTEX_ATTRIB(VERTEX_POSITION_ATTRIBUTE, 3, GL_FLOAT, offsetof(Vertex, position));
        ADD_VERTEX_ATTRIB(VERTEX_UV_ATTRIBUTE, 2, GL_FLOAT, offsetof(Vertex, uv));
        ADD_VERTEX_ATTRIB(VERTEX_COLOR_ATTRIBUTE, 3, GL_FLOAT, offsetof(Vertex, rgb));
    }, GL_ARRAY_BUFFER);

    return result;
}

GLuint createUniformBuffer(u32 size){
    GLuint result;
    glGenBuffers(1, &result);

    VERTEX_BUFFER_SCOPE(result, {
        glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    }, GL_UNIFORM_BUFFER);

    return result;
}

GLint createElementBuffer(int size){
    GLuint result;
    glGenBuffers(1, &result);

    VERTEX_BUFFER_SCOPE(result, {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    }, GL_ELEMENT_ARRAY_BUFFER);

    return result;
}

struct Texture2DArray {
    GLuint textureId;
    u32 width;
    u32 height;
    u8 depth;
    u8 texturesAmount;
};

struct TextureCube {
    GLuint textureId;
    u32 width;
    u32 height;
};

u8* downSampleTexture2x(u8* texture, u32 width, u32 height){
    u32 w = width / 2;
    u32 h = height / 2;
    u8* result = new u8[w * 3 * h];
    for(int y = 0; y < h; ++y){
        for(int x = 0; x < w * 3; ++x){
            u8 xy0 = (texture[x + y * width * 3] + texture[(x + 3) + y * width * 3]) / 2;
            u8 xy1 = (texture[x + y * width * 3] + texture[x + (y + 1) * width * 3]) / 2;
            u8 xy2 = (xy0 + xy1) / 2;
            result[x + y * w * 3] = xy2;
        }
    }
    return result;
}

GLuint createGLTextureCube(u32 width, u32 height, u8** textures){
    GLuint result;

    glGenTextures(1, &result);
    TEXTURE_SCOPE(result, {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 2);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
        for(int i = GL_TEXTURE_CUBE_MAP_POSITIVE_X; i <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; ++i){
            glTexImage2D(i, 0, GL_RGB, 
                        width, height,
                        0, 
                        GL_RGB, GL_UNSIGNED_BYTE,
                        textures[i - GL_TEXTURE_CUBE_MAP_POSITIVE_X]);
            // u8* downSampledTexture0 = downSampleTexture2x(textures[i - GL_TEXTURE_CUBE_MAP_POSITIVE_X], 
            //                                               width, height);
            // u8* downSampledTexture1 = downSampleTexture2x(downSampledTexture0, 
            //                                               width / 2, height / 2);
            // glTexImage2D(i, 1, GL_RGB, 
            //             width / 4, height / 4,
            //             0, 
            //             GL_RGB, GL_UNSIGNED_BYTE,
            //             downSampledTexture1);

            // delete[] downSampledTexture0;
            // delete[] downSampledTexture1;
        }


    }, GL_TEXTURE_CUBE_MAP);
    
    return result;
}

GLuint createGLTexture2DArray(u32 width, u32 height, u8 depth){
    GLuint result;

    glGenTextures(1, &result);

    TEXTURE_SCOPE(result, {
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, 
                     width, height, depth,
                     0, 
                     GL_RGB, GL_UNSIGNED_BYTE,
                     NULL);
    }, GL_TEXTURE_2D_ARRAY);

    return result;
}

TextureCube createTextureCube(u32 width, u32 height, u8** textures){
    TextureCube result = {};

    result.width = width;
    result.height = height;
    result.textureId = createGLTextureCube(width, height, textures);

    return result;
}

Texture2DArray createTexture2DArray(u32 width, u32 height, u8 depth){
    Texture2DArray result = {};
    result.width = width;
    result.height = height;
    result.depth = depth;
    result.textureId = createGLTexture2DArray(width, height, depth);
    return result;
}

void emplaceTextureInCube(TextureCube* textureCube, u8* data, GLenum type){
    TEXTURE_SCOPE(textureCube->textureId, {
        glTexImage2D(type, 0, GL_RGB, 
                    textureCube->width, textureCube->height,
                    0, 
                    GL_RGB, GL_UNSIGNED_BYTE,
                    data);
    }, GL_TEXTURE_CUBE_MAP);
}

void emplaceTextureIn2DArray(Texture2DArray* textureArray, u8* data, u32 offset){
    TEXTURE_SCOPE(textureArray->textureId, {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
                        0, 0, offset,
                        textureArray->width, textureArray->height, 1,
                        GL_RGB, GL_UNSIGNED_BYTE,
                        data);
    }, GL_TEXTURE_2D_ARRAY);
}

void pushTextureIn2DArray(Texture2DArray* textureArray, u8* data){
    ASSERT(textureArray->texturesAmount < 8);
    emplaceTextureIn2DArray(textureArray, data, textureArray->texturesAmount++);
}

void setTextureBinding(u8 bindingPosition, GLuint textureId, GLenum type){
    glActiveTexture(GL_TEXTURE0 + bindingPosition);
    glBindTexture(type, textureId);
}


