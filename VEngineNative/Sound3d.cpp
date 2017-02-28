#include "stdafx.h"
#include "Sound3d.h"

Sound3d::Sound3d(string mediakey, TransformationManager* itransformation)
    :Sound(mediakey)
{
    transformation = itransformation;
}


Sound3d::~Sound3d()
{
}

void Sound3d::update(Camera * listener)
{
    sound.setMinDistance(1.f);
    sound.setAttenuation(1.0f);
    auto lispos = listener->transformation->getPosition();
    auto lisdir = listener->cone->reconstructDirection(glm::vec2(0.5));
    auto lisdir2 = listener->cone->reconstructDirection(glm::vec2(0.0));
    auto crs = glm::normalize(glm::cross(lisdir, lisdir2));
    auto sndpos = transformation->getPosition();
    sound.setPosition(sf::Vector3f(sndpos.x, sndpos.y, sndpos.z));
    sf::Listener::setPosition(sf::Vector3f(lispos.x, lispos.y, lispos.z));
    sf::Listener::setDirection(sf::Vector3f(lisdir.x, lisdir.y, lisdir.z));
    sf::Listener::setUpVector(sf::Vector3f(crs.x, crs.y, crs.z));
}
