#pragma once
#include "Texture2d.h"
#include "CubeMapTexture.h"
#include "Texture2dArray.h"
class AbsFramebuffer
{
public:
    AbsFramebuffer();
    AbsFramebuffer(int iwidth, int iheight, GLuint ihandle);
    ~AbsFramebuffer();
    void attachTexture(Texture2d *tex, GLenum attachment);
    void attachTexture(CubeMapTexture *tex, GLenum attachment);
    void attachTexture(Texture2dArray *tex, GLenum attachment);
    void use(bool clear = true);
private:
    class Attachment {
    public:
        Texture2d* texture2d;
        CubeMapTexture* textureCube;
        Texture2dArray* texture2dArray;
        GLenum attachment;
    };
    GLuint handle;
    bool generated;
    int width;
    int height;
    void generateBase();
protected:
    virtual void generate() = 0;
    vector<Attachment*> attachedTextures;
};

