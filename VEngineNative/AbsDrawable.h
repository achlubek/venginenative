#pragma once
#include "AbsTransformable.h";
class AbsDrawable : public AbsTransformable
{
public:
    virtual void draw() = 0;
    virtual void setUniforms(TransformStruct transform) = 0;
};

