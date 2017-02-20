#pragma once
#include "AbsTransformable.h";
class AbsDrawable
{
public:
    virtual void draw() = 0;
    virtual void setUniforms() = 0;
};

