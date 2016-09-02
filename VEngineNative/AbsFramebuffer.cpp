#include "stdafx.h"
#include "AbsFramebuffer.h"

AbsFramebuffer::AbsFramebuffer()
{
    attachedTextures = {};
    generated = false;
    width = 1;
    height = 1;
}

AbsFramebuffer::AbsFramebuffer(int iwidth, int iheight, GLuint ihandle)
{
    handle = ihandle;
    width = iwidth;
    height = iheight;
    generated = true;
}

AbsFramebuffer::~AbsFramebuffer()
{
}

void AbsFramebuffer::attachTexture(Texture2d * tex, GLenum attachment)
{
    Attachment *ath = new Attachment();
    ath->texture2d = tex;
    ath->attachment = attachment;
    attachedTextures.push_back(ath);
    width = tex->width;
    height = tex->height;
}
void AbsFramebuffer::attachTexture(CubeMapTexture * tex, GLenum attachment)
{
    Attachment *ath = new Attachment();
    ath->textureCube = tex;
    ath->attachment = attachment;
    attachedTextures.push_back(ath);
    width = tex->width;
    height = tex->height;
}
void AbsFramebuffer::attachTexture(Texture2dArray * tex, GLenum attachment)
{
    Attachment *ath = new Attachment();
    ath->texture2dArray = tex;
    ath->attachment = attachment;
    attachedTextures.push_back(ath);
    width = tex->width;
    height = tex->height;
}

void AbsFramebuffer::use(bool clear)
{
    if (!generated)
        generateBase();

    glBindFramebuffer(GL_FRAMEBUFFER, handle);

    glViewport(0, 0, width, height);
    if (clear)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void AbsFramebuffer::generateBase()
{
    glGenFramebuffers(1, &handle);
    glBindFramebuffer(GL_FRAMEBUFFER, handle);
    vector<GLenum> buffers;
    for (int i = 0; i < attachedTextures.size(); i++) {
        if (attachedTextures[i]->texture2d != NULL) {
            attachedTextures[i]->texture2d->pregenerate();
            glFramebufferTexture(GL_FRAMEBUFFER, attachedTextures[i]->attachment, attachedTextures[i]->texture2d->handle, 0);
        }
        if (attachedTextures[i]->textureCube != NULL) {
            attachedTextures[i]->textureCube->pregenerate();
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachedTextures[i]->attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X, attachedTextures[i]->textureCube->handle, 0);
        }
        if (attachedTextures[i]->texture2dArray != NULL) {
            attachedTextures[i]->texture2dArray->pregenerate();
            glFramebufferTextureLayer(GL_FRAMEBUFFER, attachedTextures[i]->attachment, attachedTextures[i]->texture2dArray->handle, 0, 0);
        }
        if (attachedTextures[i]->attachment < GL_DEPTH_ATTACHMENT)buffers.push_back(attachedTextures[i]->attachment);
    }
    glDrawBuffers((GLsizei)buffers.size(), buffers.data());

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("AbsFramebuffer not complete");
    }
    generate();
    generated = true;
}