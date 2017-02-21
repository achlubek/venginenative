#include "stdafx.h"
#include "ShaderStorageBuffer.h"

ShaderStorageBuffer::ShaderStorageBuffer()
{
    generated = false;
}

ShaderStorageBuffer::ShaderStorageBuffer(GLuint ihandle)
{
    handle = handle;
    generated = true;
}

ShaderStorageBuffer::~ShaderStorageBuffer()
{
}

void ShaderStorageBuffer::use(unsigned int index)
{
    if (!generated) generate();
    glBindBufferBase(bufferType, index, handle);
}

void ShaderStorageBuffer::mapData(size_t size, const void * data)
{
    if (!generated) generate();
    glBindBuffer(bufferType, handle);
    glBufferData(bufferType, size, data, usageHint);
}
void ShaderStorageBuffer::mapSubData(int offset, size_t size, const void * data)
{
    if (!generated) generate();
    glBindBuffer(bufferType, handle);
    glBufferData(bufferType, size, data, usageHint);
    glBufferSubData(bufferType, offset, size, data);
}

void ShaderStorageBuffer::readSubData(int offset, size_t size, void * data)
{
    glBindBuffer(bufferType, handle);
    GLvoid* p = glMapBufferRange(bufferType, offset, size, GL_MAP_READ_BIT);
    memcpy(data, p, size);
    glUnmapBuffer(bufferType);
}

void* ShaderStorageBuffer::aquireAsynchronousPointer(int offset, size_t size)
{
    return glMapBufferRange(bufferType, offset, size, GL_MAP_UNSYNCHRONIZED_BIT);
}

void ShaderStorageBuffer::generate()
{
    glGenBuffers(1, &handle);
    generated = true;
}