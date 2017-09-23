#pragma once
class AnimationPlayer
{
public:
    TransformationManager* trans;
    float * floats = nullptr;
    int offset = 0;
    int framecount = 0;
    int frame = 0;
    int delay = 0;
    AnimationPlayer(TransformationManager* t, float * buffer, int offset, int framecount);
    ~AnimationPlayer();
    
    void nextFrame();
};

