#pragma once
class ShaderStorageBuffer
{
public:
    #define GL_SHADER_STORAGE_BUFFER 0x90D2
    #define GL_DYNAMIC_DRAW 0x88E8
    ShaderStorageBuffer();
    ShaderStorageBuffer(GLuint ihandle);
    ~ShaderStorageBuffer();
    GLenum bufferType = GL_SHADER_STORAGE_BUFFER;
    GLenum usageHint = GL_DYNAMIC_DRAW;
    void use(unsigned int index);
    void mapData(size_t size, const void * data);
    void mapSubData(int offset, size_t size, const void * data);
    void readSubData(int offset, size_t size, void * data);
    void* aquireAsynchronousPointer(int offset, size_t size);
private:
    GLuint handle;
    bool generated;
    void generate();
};
