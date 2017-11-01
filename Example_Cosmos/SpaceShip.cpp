#include "stdafx.h"
#include "SpaceShip.h"
#include "SpaceShipModule.h"


SpaceShip::SpaceShip(glm::dvec3 pos, glm::dquat orient)
    :position(pos), orientation(orient), modules({})
{

}


SpaceShip::~SpaceShip()
{
}

glm::dvec3 SpaceShip::getPosition()
{
    return position;
}

glm::dquat SpaceShip::getOrientation()
{
    return orientation;
}

glm::dvec3 SpaceShip::getLinearVelocity()
{
    return linearVelocity;
}

glm::dvec3 SpaceShip::getAngularVelocity()
{
    return angularVelocity;
}

void SpaceShip::update(double time_elapsed)
{
    for (int i = 0; i < modules.size(); i++) {
        if (modules[i]->isEnabled()) {
            modules[i]->update(this, time_elapsed);
        }
    }
    position += time_elapsed * linearVelocity;
    orientation = orientation
        * glm::angleAxis(time_elapsed * angularVelocity.x, glm::dvec3(1.0, 0.0, 0.0))
        * glm::angleAxis(time_elapsed * angularVelocity.y, glm::dvec3(0.0, 1.0, 0.0))
        * glm::angleAxis(time_elapsed * angularVelocity.z, glm::dvec3(0.0, 0.0, 1.0));
}

void SpaceShip::applyImpulse(glm::dvec3 relativePos, glm::dvec3 force)
{
    double cosine = glm::length(force) == 0 ? 0.0 : glm::dot(glm::normalize(relativePos), glm::normalize(force));
    double sine = glm::sign(cosine) * glm::sqrt(1.0 - cosine * cosine);
    double t = glm::length(relativePos) * glm::length(force) * sine;
    glm::dvec3 T = glm::cross(relativePos, force);
    auto m3 = glm::mat3_cast(orientation);
    angularVelocity += T;
    linearVelocity += m3 * (/*glm::abs(cosine) * */ force); // 100% when relativepos == force (dot == 1) or =relativepos == force (dot == -1), 0 when dot == 0
}

void SpaceShip::applyGravity(glm::dvec3 force)
{
    linearVelocity += force;
}
