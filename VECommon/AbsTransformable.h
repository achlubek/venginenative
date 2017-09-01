#pragma once
#include "TransformationManager.h"
class AbsTransformable
{
public:
    TransformationManager *transformation;
    virtual ~AbsTransformable()
    {
        delete transformation;
    }
protected:
    void initTransformation();
};
