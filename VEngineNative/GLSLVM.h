#pragma once
#include "ShaderProgram.h"
#include "ShaderStorageBuffer.h"
#include "Texture2d.h"
class GLSLVM
{
public:
    GLSLVM();
    ~GLSLVM();
    std::vector<int> programAssembly;
    std::vector<Texture2d*> textures;
    std::vector<glm::vec2> texturesScales;
    void run(int instances);
    void resizeBufferInt(int size);
    void resizeBufferFloat(int size);
    void resizeBufferVec2(int size);
    void resizeBufferVec3(int size);
    void resizeBufferVec4(int size);

    void bufferSubDataInt(int offset, int count, int* data);
    void bufferSubDataFloat(int offset, int count, float* data);
    void bufferSubDataVec2(int offset, int count, glm::vec2* data);
    void bufferSubDataVec3(int offset, int count, glm::vec3* data);
    void bufferSubDataVec4(int offset, int count, glm::vec4* data);

    void readSubDataInt(int offset, int size, int* data);
    void readSubDataFloat(int offset, int size, float* data);;
    void readSubDataVec2(int offset, int size, glm::vec2* data);
    void readSubDataVec3(int offset, int size, glm::vec3* data);
    void readSubDataVec4(int offset, int size, glm::vec4* data);
private:
    ShaderProgram* processor;
    ShaderStorageBuffer* intBuffer;
    ShaderStorageBuffer* floatBuffer;
    ShaderStorageBuffer* vec2Buffer;
    ShaderStorageBuffer* vec3Buffer;
    ShaderStorageBuffer* vec4Buffer;
};

