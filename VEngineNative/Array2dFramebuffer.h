#pragma once
#include "AbsFramebuffer.h"
class Array2dFramebuffer : public AbsFramebuffer
{
public:
    void switchLayer(int layer, bool clear);
private:
    void generate();
};
