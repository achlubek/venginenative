#pragma once

class Object3dInfo
{
public:
    Object3dInfo(vector<GLfloat> &vbo);
    ~Object3dInfo();

    GLenum drawMode;

    void draw();
    void drawInstanced(size_t instances);
    void updateRadius();
    float radius = 0.0f;
    vector<GLfloat> vbo;
    void rebufferVbo(vector<GLfloat> data, bool force_resize);

private:

    void generate();

    bool generated = false;
    GLuint vboHandle = 0, vaoHandle = 0;
    GLsizei vertexCount = 0;
};
