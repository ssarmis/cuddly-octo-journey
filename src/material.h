#pragma once

#include <unistd.h>
#include <pthread.h>

#include "general.h"
#include "math.h"
#include "gl_utilities.h"

#define MATERIALS_MAXIMUM_AMOUNT 32

// TODO(Sarmis) replace the image reading
// with a from scratch made decoder
// for know since I work on the parser, I want
// to see that the materials works properly
// so this is why I use this for now
// SPONGE
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#pragma pack(push, 1)
struct Material {
    v4 ambientLight;
    v4 specularLight;
    v4 diffuseLight;
    u32 materialFlags[4];
};
#pragma pack(pop)

enum MaterialTextureType {
    MATERIAL_TEXTURE_AMBIENT = 0,
    MATERIAL_TEXTURE_DIFFUSE,
    MATERIAL_TEXTURE_SPECULAR,
    MATERIAL_TEXTURE_NORMAL_MAP,
    MATERIAL_TEXTURE_ROUGHNESS_MAP,

    MATERIAL_TEXTURE_ARRAY_SIZE
};

struct MaterialJob;
struct MaterialJobQueue;

struct MaterialWorker {
    pthread_t id;
    bool alive;
    MaterialJobQueue* jobQueue;
};
struct MaterialJobQueue {
    pthread_mutex_t jobMutex;
    pthread_mutex_t pendingTasksMutex;
    pthread_mutex_t queueStatusMutex;
    Buffer<MaterialJob> jobs;
    Buffer<MaterialWorker*> workers;
    u32 queuedJobs;
    u32 doneJobs;
};

struct MaterialTexture {
    MaterialTextureType textureType;
    i32 width;
    i32 height;
    u8* data;
};

struct MaterialTasks {
    Buffer<MaterialTexture> pendingTexturesToLoad;
};

typedef void (*MaterialJobFunction)(MaterialJobQueue*, MaterialTasks*, String, MaterialTextureType);

struct MaterialJob {
    MaterialJobFunction task;
    // NOTE(Sarmis) they for the moment have only one
    // thing to call, so keeping the function pointer
    // is redundant, but if any other task type may appear
    // this can be reused in the future, if not
    // I will just remove the function pointer from here
    // if there is no need for some general purpose stuff
    MaterialTasks* pendingTasks;
    MaterialTextureType textureType;
    String path;
};


void* materialWorker(void* data){
    MaterialWorker* context = (MaterialWorker*)data;

    while(context->alive){
        usleep(100);
        pthread_mutex_lock(&context->jobQueue->jobMutex);
        fflush(stdout);
        if(context->jobQueue->jobs.currentAmount){
            MaterialJob job = bufferPop<MaterialJob>(&context->jobQueue->jobs);

            pthread_mutex_unlock(&context->jobQueue->jobMutex);

            job.task(context->jobQueue, job.pendingTasks, job.path, job.textureType);

            pthread_mutex_lock(&context->jobQueue->queueStatusMutex);
            ++context->jobQueue->doneJobs;
            pthread_mutex_unlock(&context->jobQueue->queueStatusMutex);
        } else {
            pthread_mutex_unlock(&context->jobQueue->jobMutex);
        }
    }
}

void appendTextureDataInPendingTasks(MaterialJobQueue* jobQueue, MaterialTasks* tasks, String path, MaterialTextureType textureType){
    MaterialTexture materialTexture = {};
    // materialTexture.data = NULL;
    BENCHMARK_SCOPE("stb load", {
        materialTexture.data = stbi_load((char*)path.data,
                                     &materialTexture.width, &materialTexture.height, NULL, 3);
    });
    materialTexture.textureType = textureType;
    if(materialTexture.data){
        pthread_mutex_lock(&jobQueue->pendingTasksMutex);
        bufferAppend<MaterialTexture>(&tasks->pendingTexturesToLoad, materialTexture);
        pthread_mutex_unlock(&jobQueue->pendingTasksMutex);
    } else {
        TRACE("Couldn't load texture from file: %.*s\n", path.size, path.data);
    }
}

void materialWorkerQueueTextureJob(MaterialJobQueue* jobQueue, MaterialJob job){
    // TODO(Sarmis) muted the buffer accesses
    job.task = appendTextureDataInPendingTasks;
    bufferAppend<MaterialJob>(&jobQueue->jobs, job);
    ++jobQueue->queuedJobs;
}

Texture2DArray materialFinishPendingTasks(MaterialTasks* pendingTasks){
    Texture2DArray result = {};
    if(pendingTasks->pendingTexturesToLoad.currentAmount){
        MaterialTexture referenceTexture = pendingTasks->pendingTexturesToLoad.array[0];
        result = createTexture2DArray(referenceTexture.width, referenceTexture.height, MATERIAL_TEXTURE_ARRAY_SIZE);
        for(int task = 0; task < pendingTasks->pendingTexturesToLoad.currentAmount; ++task){
            referenceTexture = pendingTasks->pendingTexturesToLoad.array[task];
            emplaceTextureIn2DArray(&result, referenceTexture.data, referenceTexture.textureType);
            stbi_image_free(referenceTexture.data);
        }
    } else {
        u8 data[3] = {0xff, 0xff, 0xff};
        result = createTexture2DArray(1, 1, MATERIAL_TEXTURE_ARRAY_SIZE);
        emplaceTextureIn2DArray(&result, data, MATERIAL_TEXTURE_AMBIENT);
        emplaceTextureIn2DArray(&result, data, MATERIAL_TEXTURE_DIFFUSE);
        emplaceTextureIn2DArray(&result, data, MATERIAL_TEXTURE_SPECULAR);
        emplaceTextureIn2DArray(&result, data, MATERIAL_TEXTURE_NORMAL_MAP);
        emplaceTextureIn2DArray(&result, data, MATERIAL_TEXTURE_ROUGHNESS_MAP);
    }
    return result;
}

void materialWorkerPoolStart(MaterialJobQueue* jobQueue, u8 numberOfWorkers){
    for(int i = 0; i < numberOfWorkers; ++i){
        MaterialWorker* worker = new MaterialWorker();
        worker->jobQueue = jobQueue;
        worker->alive = true;
        bufferAppend<MaterialWorker*>(&jobQueue->workers, &worker);
        
        int status = pthread_create(&worker->id, NULL, materialWorker, worker);
        if(status){
            TRACE("Failed to create material worker\n");
        }
    }
}

void materialWaitForJobsToFinish(MaterialJobQueue* jobQueue){
    while(jobQueue->doneJobs != jobQueue->queuedJobs){
        usleep(100);
    }
}

void materialKillWorkers(MaterialJobQueue* jobQueue){
    for(int i = 0; i < jobQueue->workers.currentAmount; ++i){
        jobQueue->workers.array[i]->alive = false;
        pthread_join(jobQueue->workers.array[i]->id, NULL);
    }
    bufferClean(&jobQueue->workers);
}


MaterialJobQueue materialJobQueueCreate(){
    MaterialJobQueue result = {};
    
    result.jobs.array = NULL;
    result.jobs.currentAmount = 0;
    result.jobs.capacity = 0;

    int status;
    status = pthread_mutex_init(&result.jobMutex, NULL);
    status = pthread_mutex_init(&result.pendingTasksMutex, NULL) || status;
    status = pthread_mutex_init(&result.queueStatusMutex, NULL) || status;

    if(status){
        TRACE("Failed to initialize mutexes\n");
    }

    return result;
}





