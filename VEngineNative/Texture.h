#pragma once

class Texture
{
public:
    Texture(GLuint ihandle);
    Texture(string filekey);
    Texture(int iwidth, int iheight, GLint internalFormat, GLenum format, GLenum type);
    ~Texture();



    GLuint handle;
    int components;
    int width, height;
    void use(int unit);
    void pregenerate();
    void generateMipMaps();
private:
    bool generated;
    void generate();
    unsigned char* data;
    int genMode;
    const int genModeFromFile = 1;
    const int genModeEmptyFromDesc = 2;
    void* glitextureptr;
    GLint internalFormatRequested;
    GLenum formatRequested;
    GLenum typeRequested;
};

