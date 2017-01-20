#include "stdafx.h"
#include "GLSLVM.h"

#include "Texture2d.h"

GLSLVM::GLSLVM()
{
    processor = new ShaderProgram("GLSLVM.compute.glsl");
    programAssembly = std::vector<int>();
    textures = std::vector<Texture2d*>();
    texturesScales = std::vector<glm::vec2>();
    intBuffer = new ShaderStorageBuffer();
    floatBuffer = new ShaderStorageBuffer();
    vec2Buffer = new ShaderStorageBuffer();
    vec3Buffer = new ShaderStorageBuffer();
    vec4Buffer = new ShaderStorageBuffer();
}


GLSLVM::~GLSLVM()
{
}

void GLSLVM::run(int instances)
{
    auto s = processor;
    s->use();
    intBuffer->use(0);
    floatBuffer->use(1);
    vec2Buffer->use(2);
    vec3Buffer->use(3);
    vec4Buffer->use(4);
    s->setUniformVector("AsmProgram", programAssembly);
    s->setUniformVector("TexturesScales", texturesScales);
    s->setUniform("AsmProgramLength", (int)programAssembly.size());
    s->setUniform("TexturesCount", (int)textures.size());
    for (int i = 0; i < textures.size(); i++) {
        textures[i]->use(i);
    }
    processor->dispatch(instances, 1, 1);
}

void GLSLVM::resizeBufferInt(int size)
{
    intBuffer->mapData(size, (void*)0);
}

void GLSLVM::resizeBufferFloat(int size)
{
    floatBuffer->mapData(size, (void*)0);
}

void GLSLVM::resizeBufferVec2(int size)
{
    vec2Buffer->mapData(size, (void*)0);
}

void GLSLVM::resizeBufferVec3(int size)
{
    vec3Buffer->mapData(size, (void*)0);
}

void GLSLVM::resizeBufferVec4(int size)
{
    vec4Buffer->mapData(size, (void*)0);
}

void GLSLVM::bufferSubDataInt(int offset, int count, int * data)
{
    intBuffer->mapSubData(offset, count * sizeof(int), data);
}

void GLSLVM::bufferSubDataFloat(int offset, int count, float * data)
{
    intBuffer->mapSubData(offset, count * sizeof(float), data);
}

void GLSLVM::bufferSubDataVec2(int offset, int count, glm::vec2 * data)
{
    intBuffer->mapSubData(offset, count * sizeof(float) * 2, data);
}

void GLSLVM::bufferSubDataVec3(int offset, int count, glm::vec3 * data)
{
    intBuffer->mapSubData(offset, count * sizeof(float) * 3, data);
}

void GLSLVM::bufferSubDataVec4(int offset, int count, glm::vec4 * data)
{
    intBuffer->mapSubData(offset, count * sizeof(float) * 4, data);
}

void GLSLVM::readSubDataInt(int offset, int size, int * data)
{
    intBuffer->readSubData(offset, size, data);
}

void GLSLVM::readSubDataFloat(int offset, int size, float * data)
{
    intBuffer->readSubData(offset, size, data);
}

void GLSLVM::readSubDataVec2(int offset, int size, glm::vec2 * data)
{
    intBuffer->readSubData(offset, size, data);
}

void GLSLVM::readSubDataVec3(int offset, int size, glm::vec3 * data)
{
    intBuffer->readSubData(offset, size, data);
}

void GLSLVM::readSubDataVec4(int offset, int size, glm::vec4 * data)
{
    intBuffer->readSubData(offset, size, data);
}
