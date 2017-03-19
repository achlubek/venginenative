#pragma once
class ShaderStorageBuffer
{
public:
    #define GL_SHADER_STORAGE_BUFFER 0x90D2
    #define GL_DYNAMIC_DRAW 0x88E8
    ShaderStorageBuffer();
    ShaderStorageBuffer(GLsizei prealloc_bytes);
    ShaderStorageBuffer(GLuint ihandle);
    ~ShaderStorageBuffer();
    GLenum bufferType = GL_SHADER_STORAGE_BUFFER;
    GLenum usageHint = GL_DYNAMIC_DRAW;
    void use(unsigned int index);
    void mapData(size_t size, const void * data);
    void mapSubData(int offset, size_t size, const void * data);
    void readSubData(int offset, size_t size, void * data);
    void* acquireAsynchronousPointer(int offset, size_t size);
    void unmapBuffer();
    void preallocate(GLsizei bytes, GLbitfield flags);
    void invalidate();
private:
    GLuint handle;
    GLsizei prealloc = 0;
    GLsizei currentBytesCount = 0;

    bool generated;
    void generate();
};
