#pragma once
#include "general.h"

#include "gl.h"
#include "gl_utilities.h"
#include "vertex.h"


#define VERTEX_BUFFER_SIZE MB(10)
#define INDEX_BUFFER_SIZE MB(10)

struct RenderBuffer {
    GLuint vertexArrayId;
    GLuint vertexBufferId;
    GLuint indexBufferId;
    GLuint uniformBufferId; // TODO(Sarmis) decide if this really shuld be here

    u32 vertexBufferSize;
    u32 indexBufferSize;

    Vertex* temporaryBuffer;
    u32 totalUsedIndices;
    u32 totalUsedVertices;
};

RenderBuffer createVertexArrayObject(u32 vertexBufferSize=VERTEX_BUFFER_SIZE, u32 indexBufferSize=INDEX_BUFFER_SIZE){
    RenderBuffer result = {};

    glGenVertexArrays(1, &result.vertexArrayId);
    ASSERT(result.vertexArrayId >= 0);

    result.vertexBufferSize = vertexBufferSize;
    result.indexBufferSize = indexBufferSize;

    result.totalUsedVertices = 0;
    result.totalUsedIndices = 0;

    VERTEX_ARRAY_BUFFER_SCOPE(result.vertexArrayId, {
        result.vertexBufferId = createVertexBuffer(vertexBufferSize);
        result.indexBufferId = createElementBuffer(indexBufferSize);
    });

    return result;
}

void pushVerteciesToRenderBuffer(RenderBuffer* buffer, Vertex* vertecies, u32 amount){
    VERTEX_BUFFER_SCOPE(buffer->vertexBufferId, {
        u32 dataSize = amount * sizeof(Vertex);

        u32 offset = buffer->totalUsedVertices * sizeof(Vertex);

        // TODO(Sarmis) consider having render groups of different size
        ASSERT(dataSize + offset <= buffer->vertexBufferSize);

        glBufferSubData(GL_ARRAY_BUFFER, offset, dataSize, vertecies);

        buffer->totalUsedVertices += amount;
    }, GL_ARRAY_BUFFER);
}

void pushVerteciesToRenderBuffer(RenderBuffer* buffer, Buffer<Vertex> array){
    pushVerteciesToRenderBuffer(buffer, array.array, array.currentAmount);
}

void pushIndicesToRenderBuffer(RenderBuffer* buffer, Index* indices, u32 amount){
    VERTEX_BUFFER_SCOPE(buffer->indexBufferId, {
        u32 dataSize = amount * sizeof(Index);
        
        u32 offset = buffer->totalUsedIndices * sizeof(Index);

        ASSERT(dataSize + offset <= buffer->indexBufferSize);

        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, dataSize, indices);

        buffer->totalUsedIndices += amount;
    }, GL_ELEMENT_ARRAY_BUFFER);
}

void pushIndicesToRenderBuffer(RenderBuffer* buffer, Buffer<Index> array){
    pushIndicesToRenderBuffer(buffer, array.array, array.currentAmount);
}

// NOTE(Sarmis): use temporaryBuffer before calling glBufferData
void pushVertexToRenderBuffer(RenderBuffer* buffer, Vertex vertex){
    VERTEX_BUFFER_SCOPE(buffer->vertexBufferId, {
        u32 offset = (buffer->totalUsedVertices++) * sizeof(Vertex);
        glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(Vertex), &vertex);
    }, GL_ARRAY_BUFFER);
}

void pushIndexToRenderBuffer(RenderBuffer* buffer, Index index){
    VERTEX_BUFFER_SCOPE(buffer->indexBufferId, {
        u32 offset = (buffer->totalUsedIndices++) * sizeof(Index);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, sizeof(Index), &index);
    }, GL_ELEMENT_ARRAY_BUFFER);
}

void pushQuad(RenderBuffer* renderBuffer, v3 position, v2 size, v2* uvs, v3 color=v3(1, 1, 1)){
    Vertex vertecies[4] = {
        Vertex(position,                         uvs[2], color),
        Vertex(position + v3(size.x, 0, 0),      uvs[3], color),
        Vertex(position + v3(0, size.y, 0),      uvs[0], color),
        Vertex(position + v3(size.x, size.y, 0), uvs[1], color)
    };

    Index indices[6] = {
        renderBuffer->totalUsedVertices + 0,
        renderBuffer->totalUsedVertices + 1,
        renderBuffer->totalUsedVertices + 2,

        renderBuffer->totalUsedVertices + 1,
        renderBuffer->totalUsedVertices + 3,
        renderBuffer->totalUsedVertices + 2
    };

    pushVerteciesToRenderBuffer(renderBuffer, vertecies, 4);
    pushIndicesToRenderBuffer(renderBuffer, indices, 6);
}

void flushRenderBuffer(GLenum mode, RenderBuffer* renderBuffer){
    VERTEX_ARRAY_BUFFER_SCOPE(renderBuffer->vertexArrayId, {
        glDrawElements(mode, renderBuffer->totalUsedIndices, GL_UNSIGNED_INT, 0);
    });
    renderBuffer->totalUsedIndices = 0;
    renderBuffer->totalUsedVertices = 0;
    // TODO(Sarmis) well since this is a flush function we might want to reset everything
}
 