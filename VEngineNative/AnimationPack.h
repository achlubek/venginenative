#pragma once
class AnimationPlayer;
class TransformationManager;
class AnimationPack
{
public:
    AnimationPack(std::string mediakey);
    ~AnimationPack();
    int framecount;
    std::map<std::string, int> offsets;
    float* floats = nullptr;
    AnimationPlayer* createPlayer(TransformationManager* transformation, std::string name);
};

