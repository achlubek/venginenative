#include "stdafx.h"
#include "AbsTexture.h"
#include "Media.h"
#include "Game.h"

AbsTexture::AbsTexture(GLenum itype, GLuint ihandle)
{
    handle = ihandle;
    generated = true;
    width = 1;
    height = 1;
    components = 4;
    data = nullptr;
    usedds = false;
    type = itype;
}

AbsTexture::AbsTexture(GLenum itype)
{
    generated = false;
    width = 1;
    height = 1;
    components = 4;
    data = nullptr;
    usedds = false;
    type = itype;
}

AbsTexture::AbsTexture(GLenum itype, int iwidth, int iheight, GLint internalFormat, GLenum format, GLenum formattype)
{
    width = iwidth;
    height = iheight;
    internalFormatRequested = internalFormat;
    formatRequested = format;
    typeRequested = formattype;
    type = itype;
    generated = false;
    genMode = genModeEmptyFromDesc;
}

AbsTexture::~AbsTexture()
{
    glDeleteTextures(1, &handle);
}

void AbsTexture::pregenerate()
{
    if (!generated) {
        generate();
    }
}

void AbsTexture::generateMipMaps()
{
    if (!generated) {
        generate();
    }
    glBindTexture(type, handle);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(type);
}
unsigned int upper_power_of_two(unsigned int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;

}

void * AbsTexture::read(int pixelbytecount)
{
    glBindTexture(type, handle);
    void *pixels = malloc(pixelbytecount * upper_power_of_two(width * height));
    glGetTexImage(type, 0, formatRequested, typeRequested, pixels);
    return pixels;
}

void AbsTexture::setWrapModes(GLuint s, GLuint t)
{
    if (!generated) {
        generate();
    }
    glBindTexture(type, handle);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, s);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, t);
}

void AbsTexture::setWrapModes(GLuint x, GLuint y, GLuint z)
{
    if (!generated) {
        generate();
    }
    glBindTexture(type, handle);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, x);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, y);
    glTexParameteri(type, GL_TEXTURE_WRAP_R, z);
}

void AbsTexture::use(int unit)
{
    if (!generated) {
        generate();
    }
    Game::instance->bindTexture(type, handle, unit);
}

void AbsTexture::bind(int unit, int level) {
    glBindImageTexture(unit, handle, 0, false, 0, GL_WRITE_ONLY, GL_R16F);
}
