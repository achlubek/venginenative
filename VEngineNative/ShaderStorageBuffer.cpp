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
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, handle);
}

void ShaderStorageBuffer::mapData(size_t size, const void * data)
{
    if (!generated) generate();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
}
void ShaderStorageBuffer::mapSubData(int offset, size_t size, const void * data)
{
    if (!generated) generate();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
}

void ShaderStorageBuffer::readSubData(int offset, size_t size, void * data)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
    GLvoid* p = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, offset, size, GL_READ_ONLY);
    memcpy(data, p, size);
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void ShaderStorageBuffer::generate()
{
    glGenBuffers(1, &handle);
    generated = true;
}