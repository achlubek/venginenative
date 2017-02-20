#include "stdafx.h"
#include "Texture2dArray.h"
#include "Media.h"
#include "Game.h"

Texture2dArray::Texture2dArray(GLuint ihandle)
    : AbsTexture(GL_TEXTURE_2D_ARRAY) {
    handle = ihandle;
    generated = true;
    width = 1;
    height = 1;
    levels = 1;
}

Texture2dArray::Texture2dArray(int iwidth, int iheight, int ilevels, GLint internalFormat, GLenum format, GLenum type)
    : AbsTexture(GL_TEXTURE_2D_ARRAY, iwidth, iheight, internalFormat, format, type) {
    levels = ilevels;
}

Texture2dArray::~Texture2dArray()
{
    glDeleteTextures(1, &handle);
}

void Texture2dArray::generate()
{
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D_ARRAY, handle);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormatRequested, width, height, levels);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormatRequested, width, height, levels, 0, formatRequested, typeRequested, (void*)0);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    generated = true;
}