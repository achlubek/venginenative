#include "stdafx.h"
#include "PhysicalEntity.h"


PhysicalEntity::PhysicalEntity(glm::dvec3 iposition, glm::dquat iorientation, glm::dvec3 ilinearVelocity, glm::dvec3 iangularVelocity)
    : position(iposition), orientation(iorientation), linearVelocity(ilinearVelocity), angularVelocity(iangularVelocity)
{
}

PhysicalEntity::PhysicalEntity(glm::dvec3 iposition, glm::dquat iorientation, glm::dvec3 ilinearVelocity)
    : position(iposition), orientation(iorientation), linearVelocity(ilinearVelocity)
{
}

PhysicalEntity::PhysicalEntity(glm::dvec3 iposition, glm::dquat iorientation)
    : position(iposition), orientation(iorientation)
{
}

PhysicalEntity::PhysicalEntity(glm::dvec3 iposition)
    : position(iposition)
{
}

PhysicalEntity::~PhysicalEntity()
{
}

glm::dvec3 PhysicalEntity::getPosition()
{
    return position;
}

glm::dquat PhysicalEntity::getOrientation()
{
    return orientation;
}

glm::dvec3 PhysicalEntity::predictPosition(double time_delta)
{
    return position + time_delta * linearVelocity;
}

glm::dquat PhysicalEntity::predictOrientation(double time_delta)
{
    return orientation
        * glm::angleAxis(time_delta * angularVelocity.x, glm::dvec3(1.0, 0.0, 0.0))
        * glm::angleAxis(time_delta * angularVelocity.y, glm::dvec3(0.0, 1.0, 0.0))
        * glm::angleAxis(time_delta * angularVelocity.z, glm::dvec3(0.0, 0.0, 1.0));
}

void PhysicalEntity::setPosition(glm::dvec3 v)
{
    position = v;
}

void PhysicalEntity::setOrientation(glm::dquat r)
{
    orientation = r;
}

glm::dvec3 PhysicalEntity::getLinearVelocity()
{
    return linearVelocity;
}

glm::dvec3 PhysicalEntity::getAngularVelocity()
{
    return angularVelocity;
}

void PhysicalEntity::stepEmulation(double time_delta)
{
    update(time_delta);
    position = predictPosition(time_delta);
    orientation = predictOrientation(time_delta);
}

void PhysicalEntity::applyImpulse(glm::dvec3 relativePos, glm::dvec3 force)
{
    double cosine = glm::length(force) == 0 ? 0.0 : glm::dot(glm::normalize(relativePos), glm::normalize(force));
    double sine = glm::sign(cosine) * glm::sqrt(1.0 - cosine * cosine);
    double t = glm::length(relativePos) * glm::length(force) * sine;
    glm::dvec3 T = glm::cross(relativePos, force);
    auto m3 = glm::mat3_cast(orientation);
    angularVelocity += T;
    linearVelocity += m3 * (/*glm::abs(cosine) * */ force); // 100% when relativepos == force (dot == 1) or =relativepos == force (dot == -1), 0 when dot == 0
    // it just didnt fit....
}

void PhysicalEntity::applyGravity(glm::dvec3 force)
{
    linearVelocity += force;
}

glm::dvec3 PhysicalEntity::modelSpaceToWorld(glm::dvec3 v)
{
    glm::dmat3 shipmat = glm::mat3_cast(orientation);
    return shipmat * v + position;
}
