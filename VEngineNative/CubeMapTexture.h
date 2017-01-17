#pragma once

#include "AbsTexture.h"

class CubeMapTexture : public AbsTexture
{
public:
    CubeMapTexture(GLuint ihandle);
    CubeMapTexture(string px, string py, string pz, string nx, string ny, string nz);
    CubeMapTexture(int iwidth, int iheight, GLint internalFormat, GLenum format, GLenum type);
    ~CubeMapTexture();
    
private:
    void generate();
    unsigned char* dataPX;
    unsigned char* dataPY;
    unsigned char* dataPZ;
    unsigned char* dataNX;
    unsigned char* dataNY;
    unsigned char* dataNZ;
    int genMode;
    const int genModeFromFile = 1;
    const int genModeEmptyFromDesc = 2;
};
