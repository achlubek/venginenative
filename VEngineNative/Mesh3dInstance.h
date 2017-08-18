#pragma once
#include "AbsTransformable.h"
#include "TransformationManager.h"

class Mesh3dInstance : public AbsTransformable
{
public:
    Mesh3dInstance();
    Mesh3dInstance(TransformationManager *transmgr);
    ~Mesh3dInstance();
    unsigned int id;
    bool visible = true;
};
