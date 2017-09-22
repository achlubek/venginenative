#pragma once
#include "AbsTransformable.h"
#include "TransformationManager.h"

class SpotLight : public AbsTransformable
{
public:
    SpotLight();
    SpotLight(TransformationManager *transmgr);
    ~SpotLight();
    unsigned int id;
};

