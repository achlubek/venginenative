#include "stdafx.h"
#include "CubeMapTexture.h"
#include "Media.h"
#include "Game.h"

CubeMapTexture::CubeMapTexture(GLuint ihandle)
    : AbsTexture(GL_TEXTURE_CUBE_MAP) {
    handle = ihandle;
    generated = true;
    width = 1;
    height = 1;
    components = 4;
    dataPX = nullptr;
    dataPY = nullptr;
    dataPZ = nullptr;
    dataNX = nullptr;
    dataNY = nullptr;
    dataNZ = nullptr;
}

CubeMapTexture::CubeMapTexture(string px, string py, string pz, string nx, string ny, string nz)
    : AbsTexture(GL_TEXTURE_CUBE_MAP) {
    int x, y, n;
    dataPX = stbi_load(Media::getPath(px).c_str(), &x, &y, &n, 0);
    dataPY = stbi_load(Media::getPath(py).c_str(), &x, &y, &n, 0);
    dataPZ = stbi_load(Media::getPath(pz).c_str(), &x, &y, &n, 0);
    dataNX = stbi_load(Media::getPath(nx).c_str(), &x, &y, &n, 0);
    dataNY = stbi_load(Media::getPath(ny).c_str(), &x, &y, &n, 0);
    dataNZ = stbi_load(Media::getPath(nz).c_str(), &x, &y, &n, 0);
    width = x;
    height = y;
    components = n;
    generated = false;
    genMode = genModeFromFile;
}

CubeMapTexture::CubeMapTexture(int iwidth, int iheight, GLint internalFormat, GLenum format, GLenum type)
    : AbsTexture(GL_TEXTURE_CUBE_MAP, iwidth, iheight, internalFormat, format, type) {
    genMode = genModeEmptyFromDesc;
}

CubeMapTexture::~CubeMapTexture()
{
}
void CubeMapTexture::generate()
{
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
    if (genMode == genModeFromFile) {
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

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, dataPX);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, dataPY);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, dataPZ);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, dataNX);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, dataNY);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, dataNZ);
        stbi_image_free(dataPX);
        stbi_image_free(dataPY);
        stbi_image_free(dataPZ);
        stbi_image_free(dataNX);
        stbi_image_free(dataNY);
        stbi_image_free(dataNZ);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
    else {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormatRequested, width, height, 0, formatRequested, typeRequested, (void*)0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormatRequested, width, height, 0, formatRequested, typeRequested, (void*)0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormatRequested, width, height, 0, formatRequested, typeRequested, (void*)0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormatRequested, width, height, 0, formatRequested, typeRequested, (void*)0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormatRequested, width, height, 0, formatRequested, typeRequested, (void*)0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormatRequested, width, height, 0, formatRequested, typeRequested, (void*)0);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (formatRequested == GL_DEPTH_COMPONENT && typeRequested == GL_UNSIGNED_INT)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    }

    generated = true;
}