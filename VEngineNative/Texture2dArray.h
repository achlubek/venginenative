#pragma once

#include "AbsTexture.h"

class Texture2dArray : public AbsTexture
{
public:
    Texture2dArray(GLuint ihandle);
    Texture2dArray(int iwidth, int iheight, int levels, GLint internalFormat, GLenum format, GLenum type);
    ~Texture2dArray();
    int levels;

private:
    void generate();
};
