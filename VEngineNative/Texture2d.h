#pragma once

class Texture2d
{
public:
    Texture2d(GLuint ihandle);
    Texture2d(string filekey);
    Texture2d(int iwidth, int iheight, GLint internalFormat, GLenum format, GLenum type);
    ~Texture2d();

    GLuint handle;
    int components;
    int width, height;
    void use(int unit);
    void Texture2d::bind(int unit, int level);
    void pregenerate();
    void generateMipMaps();
private:
    bool generated;
    void generate();
    unsigned char* data;
    int genMode;
    const int genModeFromFile = 1;
    const int genModeEmptyFromDesc = 2;
    string ddsFile;
    bool usedds;
    GLint internalFormatRequested;
    GLenum formatRequested;
    GLenum typeRequested;
};
