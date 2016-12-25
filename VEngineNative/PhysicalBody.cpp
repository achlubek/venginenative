#include "stdafx.h"
#include "PhysicalBody.h"
#include "Game.h"

PhysicalBody::PhysicalBody(btRigidBody * rigidBody, btCollisionShape * ishape, TransformationManager * manager)
{
    body = rigidBody;
    shape = ishape;
    transformation = manager;
    constraints = std::set<PhysicalConstraint*>();
}

PhysicalBody::~PhysicalBody()
{
}

void PhysicalBody::applyChanges()
{
    auto xyz = body->getCenterOfMassPosition();
    auto quat = body->getOrientation();
    transformation->setPosition(glm::vec3(xyz.x(), xyz.y(), xyz.z()));
    transformation->setOrientation(glm::quat(quat.x(), quat.y(), quat.z(), quat.w()));
}

void PhysicalBody::disable()
{
    if (!enabled) return;
    enabled = false;
    Game::instance->world->physics->removeBody(this);
}

void PhysicalBody::enable()
{
    if (enabled) return;
    enabled = true;
    Game::instance->world->physics->addBody(this);
}

void PhysicalBody::addConstraint(PhysicalConstraint * c)
{
    constraints.emplace(c);
}

void PhysicalBody::removeConstraint(PhysicalConstraint * c)
{
    constraints.erase(c);
}

bool PhysicalBody::isEnabled()
{
    return enabled;
}

void PhysicalBody::readChanges()
{
    body->setWorldTransform(btTransform(btQuaternion(transformation->orientation.x, transformation->orientation.y,
        transformation->orientation.z, transformation->orientation.w), 
        btVector3(transformation->position.x, transformation->position.y, transformation->position.z)));
    shape->setLocalScaling(btVector3(transformation->size.x, transformation->size.y, transformation->size.z));
}
