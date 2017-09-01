#pragma once
#include "Sound.h" 
#include "AbsTransformable.h"

class Sound3d : public Sound, public AbsTransformable
{
public:
    Sound3d(string mediakey, TransformationManager* transformation);
    ~Sound3d();
    void update(TransformationManager* listener);
};

