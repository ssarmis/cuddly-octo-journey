#pragma once

#include "general.h"
#include "gl.h"

#include "shader.sources"

// TODO(Sarmis) would be goot to use this as less as possible
#define SHADER_SCOPE(id, scope){\
    glUseProgram((id));\
    scope\
} 

struct OpenGLString {
    char buffer[KB(1)];
    GLsizei length;
};

struct UniformLocations {
    GLint matrixPerspective;
    GLint matrixTransform;
    GLint matrixView;
};

struct Shader {
    GLuint programId;
    union {
        GLuint shaders[2];
        struct {
            GLuint vertexShaderId;
            GLuint fragmentShaderId;
        };
    };
    UniformLocations locations;
};

OpenGLString shaderGetLog(GLuint shader){
    OpenGLString result = {};
    glGetShaderInfoLog(shader, KB(1), &result.length, result.buffer);
    return result;
}

OpenGLString programGetLog(GLuint program){
    OpenGLString result = {};
    glGetProgramInfoLog(program, KB(1), &result.length, result.buffer);
    return result;
}

GLuint compileShader(GLenum type, const char* source){
    ASSERT(source);
    GLuint result = glCreateShader(type);
    glShaderSource(result, 1, &source, NULL);
    
    glCompileShader(result);

    GLint compiled = GL_FALSE;
    glGetShaderiv(result, GL_COMPILE_STATUS, &compiled);

    if(!compiled){
        OpenGLString string = shaderGetLog(result);
        TRACE("Could not compile shader: \n %.*s \n", string.length, string.buffer);
        ASSERT(false);
    }

    return result;
}

GLuint createProgram(){
    return glCreateProgram();
}

GLint shaderGetUniformLocation(GLuint program, const char* name){
    GLint location = glGetUniformLocation(program, name);
    ASSERT(location != -1);
    return location;
}

GLuint createAndLinkProgram(u8 shaderAmount, GLuint* shaderIds){
    GLuint result = createProgram();

    for(int i = 0; i < shaderAmount; ++i){
        glAttachShader(result, shaderIds[i]);
    }

    glLinkProgram(result);
    GLint linked = GL_FALSE;
    glGetProgramiv(result, GL_LINK_STATUS, &linked);

    if (!linked) {
        OpenGLString string = programGetLog(result);
        TRACE("Could not link program: \n %.*s \n", string.length, string.buffer);
        ASSERT(false);
    }

    return result;
}

Shader createShader(const char* vertexShader, const char* fragmentShader){
    Shader result = {};

    result.vertexShaderId = compileShader(GL_VERTEX_SHADER, vertexShader);
    result.fragmentShaderId = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    result.programId = createAndLinkProgram(2, result.shaders);
    
    SHADER_SCOPE(result.programId, {
        result.locations.matrixPerspective = shaderGetUniformLocation(result.programId, "matrixPerspective");
        result.locations.matrixTransform = shaderGetUniformLocation(result.programId, "matrixTransform");
        result.locations.matrixView = shaderGetUniformLocation(result.programId, "matrixView");
    });
    return result;
}

Shader createShader(){
    return createShader(VERTEX_SOURCE, FRAGMENT_SOURCE);
}

Shader createShaderCursor(){
    return createShader(VERTEX_CURSOR_SOURCE, FRAGMENT_CURSOR_SOURCE);
}

void shaderSetUniform4m(GLint location, m4 matrix){
    glUniformMatrix4fv(location, 1, GL_TRUE, matrix.m);
}
