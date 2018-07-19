#include "stdafx.h"
#include "Sound3d.h"

Sound3d::Sound3d(std::string mediakey, glm::vec3 position)
    :Sound(mediakey), position(position)
{
}


Sound3d::~Sound3d()
{
}

void Sound3d::update(glm::vec3 listenerPosition, glm::quat listenerOrientation)
{
    sound.setMinDistance(1.0f);
    sound.setAttenuation(1.0f);
    auto front = glm::mat3(listenerOrientation) * glm::vec3(0.0, 0.0, -1.0);
    auto up = glm::mat3(listenerOrientation) * glm::vec3(0.0, 1.0, 0.0);
    auto sndpos = position;
    sound.setPosition(sf::Vector3f(sndpos.x, sndpos.y, sndpos.z));
    sf::Listener::setPosition(sf::Vector3f(listenerPosition.x, listenerPosition.y, listenerPosition.z));
    sf::Listener::setDirection(sf::Vector3f(front.x, front.y, front.z));
    sf::Listener::setUpVector(sf::Vector3f(up.x, up.y, up.z));
}
