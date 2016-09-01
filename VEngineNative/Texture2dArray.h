#pragma once
class Texture2dArray
{
public:
    Texture2dArray(GLuint ihandle);
    Texture2dArray(int iwidth, int iheight, int levels, GLint internalFormat, GLenum format, GLenum type);
    ~Texture2dArray();

    GLuint handle;
    int width, height, levels;
    void use(int unit);
    void bind(int unit);
    void pregenerate();
    void generateMipMaps();
private:
    bool generated;
    void generate();
    GLint internalFormatRequested;
    GLenum formatRequested;
    GLenum typeRequested;
};

