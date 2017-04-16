#include "stdafx.h"
#include "Texture3d.h"
#include "Media.h"
#include "Game.h"

Texture3d::Texture3d(GLuint ihandle)
    : AbsTexture(GL_TEXTURE_3D) {
    handle = ihandle;
    generated = true;
    width = 1;
    height = 1;
    depth = 1;
}

Texture3d::Texture3d(int iwidth, int iheight, int idepth, GLint internalFormat, GLenum format, GLenum itype)
    : AbsTexture(GL_TEXTURE_3D, iwidth, iheight, internalFormat, format, itype) {
 
    depth = idepth;
}

Texture3d::~Texture3d()
{
    glDeleteTextures(1, &handle);
}
void Texture3d::generate()
{
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_3D, handle); 
    glTexImage3D(GL_TEXTURE_3D, 0, internalFormatRequested, width, height, depth, 0, formatRequested, typeRequested, (void*)0); 
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    

    generated = true;
}