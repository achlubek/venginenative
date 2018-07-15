#include "stdafx.h"
#include "Sound3d.h"

Sound3d::Sound3d(std::string mediakey, TransformationManager* itransformation)
    :Sound(mediakey)
{
    transformation = itransformation;
}


Sound3d::~Sound3d()
{
}

void Sound3d::update(TransformationManager * listener)
{
    sound.setMinDistance(1.f);
    sound.setAttenuation(1.0f);
    auto pos = listener->getPosition();
    auto front = glm::mat3(listener->getRotationMatrix()) * glm::vec3(0.0, 0.0, -1.0);
    auto up = glm::mat3(listener->getRotationMatrix()) * glm::vec3(0.0, 1.0, 0.0);
    auto sndpos = transformation->getPosition();
    sound.setPosition(sf::Vector3f(sndpos.x, sndpos.y, sndpos.z));
    sf::Listener::setPosition(sf::Vector3f(pos.x, pos.y, pos.z));
    sf::Listener::setDirection(sf::Vector3f(front.x, front.y, front.z));
    sf::Listener::setUpVector(sf::Vector3f(up.x, up.y, up.z));
}
