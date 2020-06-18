#pragma once

#include "general.h"
#include "vertex.h"
#include "material.h"

struct Mesh {
    Buffer<Vertex> vertices;
    Buffer<Index> indices;
    // TODO(Sarmis) maybe move this in some mesh group
    // TODO(Sarmis) make this a buffer like the others
    Material* materials;
};

struct MeshGroup {
    Buffer<Mesh> meshes;
    // Buffer<Materials> materials;
    // TODO(Sarmis) do we need to store the materials when we load 
    // meshes to reference them to use the correct materials from 
    // the material storage or just buffer them in the material storage
    // although i need the size of the buffer so I can allocate it
    // ShaderUniformBuffer materialStorage;
};
