#include "stdafx.h"
#include "Object3dInfo.h"

/*
This class expected interleaved buffer in format
pos.xyz-uv.xy-normal.xyz-tangent.xyzw
totals in 12 elements per vertex
*/

Object3dInfo::Object3dInfo(vector<GLfloat> &vboin)
{
    vbo = move(vboin);
    generated = false;
    vertexCount = (GLsizei)(vbo.size() / 12);
    updateAABB();
    drawMode = GL_TRIANGLES;
}

Object3dInfo::~Object3dInfo()
{
    vbo.clear();
    if (generated) {
        glDeleteVertexArrays(1, &vaoHandle);
        glDeleteBuffers(1, &vboHandle);
    }
}

void Object3dInfo::draw()
{
    if (!generated)
        generate();
    glBindVertexArray(vaoHandle);
    glDrawArrays(drawMode, 0, vertexCount);
}

void Object3dInfo::drawInstanced(size_t instances)
{
    if (!generated)
        generate();
    glBindVertexArray(vaoHandle);
    glDrawArraysInstanced(drawMode, 0, vertexCount, (GLsizei)instances);
}

void Object3dInfo::updateAABB()
{
    // px py pz ux uy nx ny nz tx ty tz tw | px
    // 0  1  2  3  4  5  6  7  8  9  10 11 | 12
    int size = vbo.size();
    if (vbo.size() < 2) return;
    aabbmin = glm::vec3(vbo[0], vbo[1], vbo[2]);
    aabbmax = glm::vec3(vbo[0], vbo[1], vbo[2]);
    for (int i = 0; i < size; i += 12) {
        glm::vec3 p = glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]);
        aabbmin = glm::min(p, aabbmin);
        aabbmax = glm::max(p, aabbmax);
    }
}

void Object3dInfo::rebufferVbo(vector<GLfloat> data, bool force_resize)
{
    if (!generated) {
        vbo = data;
        return; 
    }
    if (force_resize) {
        vbo = data;
        generated = false;
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_ELEMENT_ARRAY_BARRIER_BIT);
    }
}

void Object3dInfo::generate()
{
    if (vaoHandle > 0) glDeleteVertexArrays(1, &vaoHandle);
    if (vboHandle > 0) glDeleteBuffers(1, &vboHandle);
    glGenVertexArrays(1, &vaoHandle);
    glGenBuffers(1, &vboHandle);
    glBindVertexArray(vaoHandle);
    glBindBuffer(GL_ARRAY_BUFFER, vboHandle);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vbo.size(), vbo.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(GLfloat) * 12, (void*)(sizeof(GLfloat) * 0));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(GLfloat) * 12, (void*)(sizeof(GLfloat) * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(GLfloat) * 12, (void*)(sizeof(GLfloat) * 5));
    glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(GLfloat) * 12, (void*)(sizeof(GLfloat) * 8));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    generated = true;
}