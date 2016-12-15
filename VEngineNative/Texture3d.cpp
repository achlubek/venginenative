#include "stdafx.h"
#include "Texture3d.h"
#include "Media.h"
#include "Game.h"

Texture3d::Texture3d(GLuint ihandle)
{
    handle = ihandle;
    generated = true;
    width = 1;
    height = 1;
    depth = 1;
}

Texture3d::Texture3d(int iwidth, int iheight, int idepth, GLint internalFormat, GLenum format, GLenum type)
{
    width = iwidth;
    height = iheight;
    depth = idepth;
    internalFormatRequested = internalFormat;
    formatRequested = format;
    typeRequested = type;
    generated = false;
}

Texture3d::~Texture3d()
{
    glDeleteTextures(1, &handle);
}
void Texture3d::pregenerate()
{
    if (!generated) {
        generate();
    }
}

void Texture3d::generateMipMaps()
{
    glBindTexture(GL_TEXTURE_3D, handle);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_3D);
}

void Texture3d::use(int unit)
{
    if (!generated) {
        generate();
    }
    Game::instance->bindTexture(GL_TEXTURE_3D, handle, unit);
}

void Texture3d::bind(int unit) {
    glBindImageTexture(unit, handle, 0, true, 0, GL_WRITE_ONLY, GL_R16F);
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