#pragma once
class ShaderStorageBuffer
{
public:
    ShaderStorageBuffer();
    ShaderStorageBuffer(GLuint ihandle);
    ~ShaderStorageBuffer();
    void use(unsigned int index);
    void mapData(size_t size, const void * data);
    void mapSubData(int offset, size_t size, const void * data);
    void readSubData(int offset, size_t size, void * data);
private:
    GLuint handle;
    bool generated;
    void generate();
};
