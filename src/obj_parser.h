#pragma once

#include <unistd.h>

#include "general.h"
#include "vertex.h"
#include "file.h"
#include "mesh.h"

// TODO(Sarmis) remove this from here
// SPONGE
#include "gl_utilities.h" 


u32 hash(String string){
    u32 fnvPrime = 0x01000193;
    u32 fnvOffsetBasis = 0x811c9dc5;

    u32 result = fnvOffsetBasis;
    u32 size = string.size;
    while(size){
        result ^= string.data[string.size - size];
        result *= fnvPrime;
        --size;
    }

    return result;
}

Material* createDefaultMaterials(){
    Material* result = new Material[MATERIALS_MAXIMUM_AMOUNT];
    for(int i = 0; i < MATERIALS_MAXIMUM_AMOUNT; ++i){
        result[i].ambientLight = v4(1, 1, 1, 1);
        result[i].specularLight = v4(1, 1, 1, 1);
        result[i].diffuseLight = v4(1, 1, 1, 1);
    }
    return result;
}

// reference https://www.fileformat.info/format/material/
static inline Material* parseMtlToMaterial(String filename, MaterialJobQueue* jobQueue, MaterialTasks* materialTasks){
    Material* result = createDefaultMaterials();
    // TODO(Sarmis) change the file api so we can handle files
    // that can't be found nicely
    File material = fileRead((char*)filename.data);
    String key;
    Material currentMaterial = {};
    bool firstMaterialVisited = false;

    u32 lastDelimiterIndex = characterLastOccurence(filename, '/');
    String currentDirectory = cloneString(filename.data, lastDelimiterIndex);

    bool isFirstTexture = true;
    MaterialJob job = {};

    while(material.pointer < material.size){
        char character = material.data[material.pointer++];
        switch(character){
            case 'b': {
                    skipLineInFile(&material);
                }
                break;

            case 'i': {
                    skipLineInFile(&material);
                }
                break;

            case '#': {
                    skipLineInFile(&material);
                }
                break;

            case 'N': {
                    char nextCharacter = material.data[material.pointer++];
                    switch(nextCharacter){
                        case 's':{
                                // Specifies the specular exponent for the current material. This defines the focus of the specular highlight.
                            }
                            break;

                        case 'i': {
                                // Specifies the optical density for the surface. This is also known as index of refraction.
                            }
                            break;
                    }
                }
                break;

            case 'n': {
                    // TODO(Sarmis) check string for newmtl
                    skipWordInFile(&material);
                    if(firstMaterialVisited){
                        u32 index = hash(key) % MATERIALS_MAXIMUM_AMOUNT;
                        result[index] = currentMaterial;
                        TRACE("Indexed material at %d\n", index);
                    } else {
                        firstMaterialVisited = true;
                    }
                    
                    key = readStringFromFile(&material);
                    
                    skipLineInFile(&material);
                }
                break;
                // TODO(Sarmis) implement reading ambient, specular, etc elements

                case 'm': {
                        String identifier = readStringFromFile(&material);
                        u16 delimiterIndex = characterFirstOccurence(identifier, '_');

                        String keyword = subString(identifier, delimiterIndex + 1, identifier.size);

                        String path = {};

                        i32 width;
                        i32 height;
                        i32 components;
                        u8* data = NULL;;

                        switch (keyword.data[0]) {
                            // TODO(Sarmis) will put things in proper functions later
                            case 'b': case 'B': {
                                    // NOTE(Sarmis) bumb map
                                    path = readStringFromFile(&material);
                                    path = currentDirectory + "/" + path;

                                    // TODO(Sarmis) read arguments, if any
                                    job.path = path;
                                    job.pendingTasks = materialTasks;
                                    job.textureType = MATERIAL_TEXTURE_NORMAL_MAP;
                                    materialWorkerQueueTextureJob(jobQueue, job);
                                }
                                break;

                            case 'N': {
                                    path = readStringFromFile(&material);
                                    path = currentDirectory + "/" + path;

                                    // TODO(Sarmis) read arguments, if any
                                    job.path = path;
                                    job.pendingTasks = materialTasks;
                                    job.textureType = MATERIAL_TEXTURE_NORMAL_MAP;
                                    materialWorkerQueueTextureJob(jobQueue, job);
                                }
                                break;

                            case 'K': {
                                    char nextCharacter = keyword.data[1];
                                    switch(nextCharacter){
                                        case 'd': {
                                                // NOTE(Sarmis) diffuse map
                                                path = readStringFromFile(&material);
                                                path = currentDirectory + "/" + path;

                                                // TODO(Sarmis) read arguments, if any
                                                job.path = path;
                                                job.pendingTasks = materialTasks;
                                                job.textureType = MATERIAL_TEXTURE_DIFFUSE;
                                                materialWorkerQueueTextureJob(jobQueue, job);
                                            }
                                            break;

                                        case 'a': {
                                                // NOTE(Sarmis) ambient map
                                                path = readStringFromFile(&material);
                                                path = currentDirectory + "/" + path;

                                                // TODO(Sarmis) read arguments, if any
                                                job.path = path;
                                                job.pendingTasks = materialTasks;
                                                job.textureType = MATERIAL_TEXTURE_AMBIENT;
                                                materialWorkerQueueTextureJob(jobQueue, job);
                                            }
                                            break;

                                        case 's': {
                                                // NOTE(Sarmis) specular map
                                                path = readStringFromFile(&material);
                                                path = currentDirectory + "/" + path;

                                                // TODO(Sarmis) read arguments, if any
                                                job.path = path;
                                                job.pendingTasks = materialTasks;
                                                job.textureType = MATERIAL_TEXTURE_SPECULAR;
                                                materialWorkerQueueTextureJob(jobQueue, job);
                                            }
                                            break;

                                        default: { 
                                            }
                                            break;
                                    }
                                }
                                break;
                            
                            default: {

                                }
                                break;
                        }
                        skipLineInFile(&material);
                    }
                    break;

                case 'K': {
                        char nextCharacter = material.data[material.pointer++];
                        switch(nextCharacter){
                            case 'a': {
                                    currentMaterial.ambientLight.x = readR32FromFile(&material);
                                    currentMaterial.ambientLight.y = readR32FromFile(&material);
                                    currentMaterial.ambientLight.z = readR32FromFile(&material);
                                    skipLineInFile(&material);
                                }
                                break;

                            case 'd': {
                                    currentMaterial.diffuseLight.x = readR32FromFile(&material);
                                    currentMaterial.diffuseLight.y = readR32FromFile(&material);
                                    currentMaterial.diffuseLight.z = readR32FromFile(&material);
                                    skipLineInFile(&material);
                                }
                                break;

                            case 's': {
                                    currentMaterial.specularLight.x = readR32FromFile(&material);
                                    currentMaterial.specularLight.y = readR32FromFile(&material);
                                    currentMaterial.specularLight.z = readR32FromFile(&material);
                                    skipLineInFile(&material);
                                }
                                break;
                        }
                    }
                    break;

            default: {
                }
                break;
        }
    }

    u32 index = hash(key) % MATERIALS_MAXIMUM_AMOUNT;
    result[index] = currentMaterial;
    TRACE("Indexed material at %d\n", index);

    fileClear(&material);

    return result;
}

#define ROTATE_OBJ_INDEX(index, size) ((size) + (index))

// reference http://www.martinreddy.net/gfx/3d/OBJ.spec
static inline Buffer<Mesh> parseObjToMeshes(const char* filename, MaterialTasks* materialTasks){
    Buffer<Mesh> result = {};

    File model = fileRead(filename);

    v3 defaultV3 = v3();
    v2 defaultV2 = v2();

    Buffer<v3> positions = {};
    Buffer<v2> uvs = {};
    Buffer<v3> normals = {};

    bufferAppend<v3>(&positions, &defaultV3);
    bufferAppend<v2>(&uvs, &defaultV2);
    bufferAppend<v3>(&normals, &defaultV3);
    
    Index indexAmount = 0;

    Mesh currentMesh = {};

    MaterialJobQueue jobQueue = materialJobQueueCreate();

#if 1
    u8 numberOfCores = sysconf(_SC_NPROCESSORS_ONLN);
    materialWorkerPoolStart(&jobQueue, numberOfCores);
#else
    materialWorkerPoolStart(&jobQueue, 1);
#endif
    Material* materials = createDefaultMaterials();

    bool fistObjectVisited = false;
    u32 currentMaterialIndex = 0;

    while(model.pointer < model.size){
        char character = model.data[model.pointer++];

        switch (character){
            case '#':{
                    skipLineInFile(&model);
                }
                break;

            case 'o': { 
                    indexAmount = 0;
                    if(fistObjectVisited){
                        currentMesh.materials = materials;
                        bufferAppend<Mesh>(&result, &currentMesh);
                        
                        bufferCleanSafe<Index>(&currentMesh.indices);
                        bufferCleanSafe<Vertex>(&currentMesh.vertices);
                    } else {
                        fistObjectVisited = true;
                    }
                    // TODO(Sarmis) store the name?
                    skipLineInFile(&model);
                }
                break;

            // TODO(Sarmis) maybe implement grouping for objects?
            // materials are for objects, not for groups so for now
            // no need for that
            case 'g': { 
                    skipLineInFile(&model);
                }
                break;

            case 's': { 
                    skipLineInFile(&model);
                }
                break;

            case 'm': {
                    skipWordInFile(&model);
                    String materialFilename = readStringFromFile(&model);

                    String path = cloneString(filename);
                    u32 lastDelimiterIndex = characterLastOccurence(path, '/');
                    path = cloneString(path.data, lastDelimiterIndex);
                    path += "/" + materialFilename;

                    materials = parseMtlToMaterial(path, &jobQueue, materialTasks);

                    skipLineInFile(&model);
                }
                break;

            case 'u': {
                    skipWordInFile(&model);
                    String materialName = readStringFromFile(&model);
                    currentMaterialIndex = hash(materialName) % MATERIALS_MAXIMUM_AMOUNT;
                    skipLineInFile(&model);
                }
                break;

            case 'f': {
                    
                    // TODO(Sarmis) add to handle v/vt without any vn
                    u8 faceCount = 0;
                    u32 indexStart = indexAmount;
                    while(true){
                        i32 positionIndex = readI32FromFile(&model);
                        if(positionIndex < 0){
                            positionIndex = ROTATE_OBJ_INDEX(positionIndex, positions.currentAmount);
                        }

                        ++model.pointer;
                        i32 uvIndex = readI32FromFile(&model);
                        if(uvIndex < 0){
                            uvIndex = ROTATE_OBJ_INDEX(uvIndex, uvs.currentAmount);
                        }

                        ++model.pointer;
                        i32 normalIndex = readI32FromFile(&model);
                        if(normalIndex < 0){
                            normalIndex = ROTATE_OBJ_INDEX(normalIndex, normals.currentAmount);
                        }

                        Vertex vertex = {};
                        vertex.position = positions.array[positionIndex];
                        vertex.uv = uvs.array[uvIndex];
                        vertex.normal =  normals.array[normalIndex];
                        vertex.materialIndex = currentMaterialIndex;
                        
                        bufferAppend<Vertex>(&currentMesh.vertices, &vertex);
                        
                        ++indexAmount;

                        skipSpacesInFile(&model);

                        if(model.data[model.pointer] == '\n' || 
                           model.data[model.pointer] == '\r'){
                            break;
                        }
                    }

                    // TODO(Sarmis) make this work for concav faces as well
                    // NOTE(Sarmis) for the moment I don't think we need more than 255 points
                    // on a face
                    u8 pointsOnFace = indexAmount - indexStart;
                    int numberOfTriangles = pointsOnFace - 2;
                    i16 offset = 0;
                    for(int i = 0; i < numberOfTriangles; ++i){
                        bufferAppend<Index>(&currentMesh.indices, indexStart);
                        bufferAppend<Index>(&currentMesh.indices, indexStart + offset + 1);
                        bufferAppend<Index>(&currentMesh.indices, indexStart + offset + 2);
                        ++offset;
                    }

                    skipLineInFile(&model);
                }
                break;

            case 'v': {
                    char nextCharacter = model.data[model.pointer++];
                    switch(nextCharacter){
                        case 't': { // its a uv coordinate
                                // NOTE(Sarmis) based on the reference only
                                // the xy coordinates are mandatory
                                // for the moment we don't have any texture depth
                                // that needs to be read from the file
                                v2 uv = {};
                                uv.x = readR32FromFile(&model);
                                uv.y = 1 - readR32FromFile(&model);
                                skipLineInFile(&model);

                                bufferAppend<v2>(&uvs, &uv);
                            }
                            break;

                        case 'n': { // its a normal
                                v3 normal = {};
                                normal.x = readR32FromFile(&model);
                                normal.y = readR32FromFile(&model);
                                normal.z = readR32FromFile(&model);
                                skipLineInFile(&model);

                                bufferAppend<v3>(&normals, &normal);
                            }
                            break;
                        
                        case ' ': { // its a position
                                // NOTE(Sarmis) based on the reference only
                                // the xyz coordinates are mandatory
                                // the w component(which is not always present) can be ignore at reading
                                // we wouldn't pass it though the pipeline anyway
                                v3 position = {};
                                position.x = readR32FromFile(&model);
                                position.y = readR32FromFile(&model);
                                position.z = readR32FromFile(&model);
                                skipLineInFile(&model);

                                bufferAppend<v3>(&positions, &position);
                            }   
                            break;
                        }
                }
                break;

            default:{
                }
                break;
        }
    }

    currentMesh.materials = materials;
    bufferAppend<Mesh>(&result, &currentMesh);

    bufferClean<v3>(&positions);
    bufferClean<v2>(&uvs);
    bufferClean<v3>(&normals);

    fileClear(&model);

    // TODO(Sarmis) wait for jobs to be over
    materialWaitForJobsToFinish(&jobQueue);
    materialKillWorkers(&jobQueue);

    return result;
}