#pragma once
#include "Sound.h"
#include "Camera.h"
#include "AbsTransformable.h"

class Sound3d : public Sound, public AbsTransformable
{
public:
    Sound3d(string mediakey, TransformationManager* transformation);
    ~Sound3d();
    void update(Camera* listener);
};

