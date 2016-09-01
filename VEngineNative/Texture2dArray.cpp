#include "stdafx.h"
#include "Texture2dArray.h"
#include "Media.h"

Texture2dArray::Texture2dArray(GLuint ihandle)
{
    handle = ihandle;
    generated = true;
    width = 1;
    height = 1;
    levels = 1;
}

Texture2dArray::Texture2dArray(int iwidth, int iheight, int ilevels, GLint internalFormat, GLenum format, GLenum type)
{
    width = iwidth;
    height = iheight;
    levels = ilevels;
    internalFormatRequested = internalFormat;
    formatRequested = format;
    typeRequested = type;
    generated = false;
}

Texture2dArray::~Texture2dArray()
{
}
void Texture2dArray::pregenerate()
{
    if (!generated) {
        generate();
    }
}

void Texture2dArray::generateMipMaps()
{
    glBindTexture(GL_TEXTURE_2D_ARRAY, handle);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

void Texture2dArray::use(int unit)
{
    if (!generated) {
        generate();
    }
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, handle);
}

void Texture2dArray::bind(int unit) {
    glBindImageTexture(unit, handle, 0, true, 0, GL_WRITE_ONLY, GL_R16F);
}

void Texture2dArray::generate()
{
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D_ARRAY, handle);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormatRequested, width, height, levels, 0, formatRequested, typeRequested, (void*)0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    generated = true;
}