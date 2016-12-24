#pragma once

class Object3dInfo
{
public:
    Object3dInfo(vector<GLfloat> &vbo);
    ~Object3dInfo();

    GLenum drawMode;

    void draw();
    void drawInstanced(size_t instances);

private:

    void generate();

    vector<GLfloat> vbo;
    bool generated = false;
    GLuint vboHandle, vaoHandle;
    GLsizei vertexCount;
};
