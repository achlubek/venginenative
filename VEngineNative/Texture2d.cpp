#include "stdafx.h"
#include "Texture2d.h"
#include "Media.h"
#include "Game.h"
#include "lupng.h"

Texture2d::Texture2d(GLuint ihandle)
    :AbsTexture(GL_TEXTURE_2D) {
    handle = ihandle;
    generated = true;
    width = 1;
    height = 1;
    components = 4;
    genMode = genModeEmptyFromDesc;
    data = nullptr;
    usedds = false;
}

Texture2d::Texture2d(string filekey)
    :AbsTexture(GL_TEXTURE_2D) {
    if (strstr(filekey.c_str(), ".dds") != nullptr) {
        ddsFile = filekey;
        usedds = true;
        genMode = genModeFromFile;
    }
    else {
        ispng = (strstr(filekey.c_str(), ".png") != nullptr);
        int x, y, n;
        if (!ispng) {
            data = stbi_load(Media::getPath(filekey).c_str(), &x, &y, &n, 0);
            width = x;
            height = y;
            components = n;
        }
        else {
            auto img = luPngReadFile(Media::getPath(filekey).c_str());
            data = img->data; 
            width = img->width;
            height = img->height;
            components = img->channels;
            is16bitpng = img->depth > 8;
        }
        generated = false;
        genMode = genModeFromFile;
        usedds = false;
    }
}

Texture2d::Texture2d(int iwidth, int iheight, GLint internalFormat, GLenum format, GLenum type)
    : AbsTexture(GL_TEXTURE_2D, iwidth, iheight, internalFormat, format, type) {
}

Texture2d::~Texture2d()
{

}

void Texture2d::generate()
{
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    if (genMode == genModeFromFile) {
        if (usedds) {
            auto glitexture = gli::load(Media::getPath(ddsFile));
            gli::gl GL(gli::gl::PROFILE_GL33);
            gli::gl::format const Format = GL.translate(glitexture.format(), glitexture.swizzles());
            // GLenum Target = GL.translate(Texture2d.target());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(glitexture.levels() - 1));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
            glm::tvec3<GLsizei> const Extent(glitexture.extent());
            glTexStorage2D(
                GL_TEXTURE_2D, static_cast<GLint>(glitexture.levels()), Format.Internal,
                Extent.x, Extent.y);
            if (gli::is_compressed(glitexture.format())) {
                glCompressedTexSubImage2D(
                    GL_TEXTURE_2D, static_cast<GLint>(0),
                    0, 0,
                    Extent.x,
                    Extent.y,
                    Format.Internal, static_cast<GLsizei>(glitexture.size(0)),
                    glitexture.data(0, 0, 0));
            }
            else {
                glTexSubImage2D(
                    GL_TEXTURE_2D, static_cast<GLint>(0),
                    0, 0,
                    Extent.x,
                    Extent.y,
                    Format.External, Format.Type,
                    glitexture.data(0, 0, 0));
            }
        }
        else {
            GLint internalFormat;
            GLenum format;
            if (components == 1) {
                internalFormat = GL_RED;
                format = GL_RED;
            }
            else if (components == 2) {
                internalFormat = GL_RG;
                format = GL_RG;
            }
            else if (components == 3) {
                internalFormat = GL_RGB;
                format = GL_RGB;
            }
            else {
                internalFormat = GL_RGBA;
                format = GL_RGBA;
            }
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, is16bitpng ? GL_UNSIGNED_SHORT : GL_UNSIGNED_BYTE, data);
            if(!ispng)stbi_image_free(data);
            else free(data);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        GLfloat largest_supported_anisotropy;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);
    }
    else {
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormatRequested, width, height, 0, formatRequested, typeRequested, (void*)0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (formatRequested == GL_DEPTH_COMPONENT)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    }

    generated = true;
}