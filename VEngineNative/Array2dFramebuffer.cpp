#include "stdafx.h"
#include "Array2dFramebuffer.h"

void Array2dFramebuffer::switchLayer(int layer, bool clear)
{
    for (int i = 0; i < attachedTextures.size(); i++) {
        if (attachedTextures[i]->texture2dArray != NULL) {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, attachedTextures[i]->attachment, attachedTextures[i]->texture2dArray->handle, 0, layer);
        }
    }
    if (clear)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Array2dFramebuffer::generate()
{
}
