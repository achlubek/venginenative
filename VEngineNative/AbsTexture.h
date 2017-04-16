#pragma once
class AbsTexture
{
public:
    AbsTexture(GLenum itype, GLuint ihandle);
    AbsTexture(GLenum itype);
    AbsTexture(unsigned char* data);
    AbsTexture(GLenum itype, int iwidth, int iheight, GLint internalFormat, GLenum format, GLenum formattype);
    ~AbsTexture();

    GLuint handle = 0;
    GLenum type;
    int components;
    int width, height;
    void use(int unit);
    void clear();
    void bind(int unit, int level, bool layered, GLenum access, GLenum format);
    void pregenerate();
    void generateMipMaps();
    void * read(int pixelbytecount);
    void setFiltering(GLenum min, GLenum max);
    void setWrapModes(GLuint x, GLuint y);
    void setWrapModes(GLuint x, GLuint y, GLuint z);
    virtual void generate() = 0;
protected:
    bool generated;
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

