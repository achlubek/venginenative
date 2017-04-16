#pragma once

#include "AbsTexture.h"

class Texture3d : public AbsTexture
{
public:
    Texture3d(GLuint ihandle);
    Texture3d(int iwidth, int iheight, int idepth, GLint internalFormat, GLenum format, GLenum type);
    ~Texture3d();
     
    int depth;
private:
    void generate();
};
