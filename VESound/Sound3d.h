#pragma once
#include "Sound.h" 
#include "AbsTransformable.h"

class Sound3d : public Sound, public AbsTransformable
{
public:
    Sound3d(std::string path, TransformationManager* transformation);
    ~Sound3d();
    void update(TransformationManager* listener);
};

