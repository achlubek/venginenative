#pragma once
#include "Sound.h" 

class Sound3d : public Sound
{
public:
    Sound3d(std::string mediakey, glm::vec3 position);
    ~Sound3d();
    void update(glm::vec3 listenerPosition, glm::quat listenerOrientation);

private:
    glm::vec3 position;
};

