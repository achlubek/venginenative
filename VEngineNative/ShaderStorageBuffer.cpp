#include "stdafx.h"
#include "ShaderStorageBuffer.h"

ShaderStorageBuffer::ShaderStorageBuffer()
{
    generated = false;
}

ShaderStorageBuffer::ShaderStorageBuffer(GLsizei bytes)
{
    prealloc = bytes;
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
    glBindBufferRange(bufferType, index, handle, 0, currentBytesCount);
}

void ShaderStorageBuffer::mapData(size_t size, const void * data)
{
    if (!generated) generate();
    glBindBuffer(bufferType, handle);
    currentBytesCount = size;
    glBufferData(bufferType, size, data, usageHint);
}
void ShaderStorageBuffer::mapSubData(int offset, size_t size, const void * data)
{
    if (!generated) generate();
    glBindBuffer(bufferType, handle);
    currentBytesCount = offset + size;
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
    if (!generated) generate();
    glBindBuffer(bufferType, handle);
    if (offset + size > currentBytesCount) preallocate(size + offset, usageHint);
    return glMapBufferRange(bufferType, offset, size, GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT);
}

void ShaderStorageBuffer::unmapBuffer()
{
    glUnmapBuffer(bufferType);
}

void ShaderStorageBuffer::invalidate()
{
    glBindBuffer(bufferType, handle);
    glInvalidateBufferData(handle);
    currentBytesCount = 0;
}

void ShaderStorageBuffer::preallocate(int bytes, GLbitfield flags)
{
    if (!generated) generate();
    glBindBuffer(bufferType, handle);
    glBufferData(bufferType, bytes, (void*)0, flags);
    currentBytesCount = bytes;
}

void ShaderStorageBuffer::generate()
{
    glGenBuffers(1, &handle);
    generated = true;
    if (prealloc > 0) preallocate(prealloc, usageHint);
}