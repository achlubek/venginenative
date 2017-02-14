#pragma once

#include "AbsTexture.h"

class Texture2d : public AbsTexture
{
public:
    Texture2d(GLuint ihandle);
    Texture2d(string filekey);
    Texture2d(int iwidth, int iheight, GLint internalFormat, GLenum format, GLenum type);
    ~Texture2d();
private:
    bool ispng = false;
    bool is16bitpng = false;

    void generate();
};
