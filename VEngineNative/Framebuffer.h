#pragma once
#include "AbsFramebuffer.h"
class Framebuffer : public AbsFramebuffer
{
    using AbsFramebuffer::AbsFramebuffer;
private:
    void generate();
};
