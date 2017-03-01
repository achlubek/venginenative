#include "stdafx.h"
#include "PhysicalBody.h"
#include "Game.h"

PhysicalBody::PhysicalBody(btRigidBody * rigidBody, btCollisionShape * ishape, TransformationManager * manager)
{
    isStaticObject = false;
    body = rigidBody;
    shape = ishape;
    transformation = manager;
    constraints = std::set<PhysicalConstraint*>();
}

PhysicalBody::PhysicalBody(btCollisionObject * rigidBody, btCollisionShape * ishape, TransformationManager * manager)
{
    isStaticObject = true;
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
    if (!isStaticObject) {
        auto xyz = ((btRigidBody*)body)->getCenterOfMassPosition();
        auto quat = ((btRigidBody*)body)->getOrientation();
        transformation->setPosition(glm::vec3(xyz.x(), xyz.y(), xyz.z()));
        float X = quat.getX();
        float Y = quat.getY();
        float Z = quat.getZ();
        float W = quat.getW();
        glm::quat qt = glm::quat(X, Y, Z, W);
        qt.x = X;
        qt.y = Y;
        qt.z = Z;
        qt.w = W; // fuck you glm::quat
        transformation->setOrientation(qt);
    }
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
    glm::quat o = transformation->getOrientation();
    glm::vec3 p = transformation->getPosition();
    glm::vec3 s = transformation->getSize();
    auto q = btQuaternion(o.x, o.y, o.z, o.w);
    auto v = btVector3(p.x, p.y, p.z);

    auto bt = btTransform();
    bt.setOrigin(v);
    bt.setRotation(q);
    body->setWorldTransform(bt);
    shape->setLocalScaling(btVector3(s.x, s.y, s.z));
}

bool PhysicalBody::isStatic()
{
    return isStaticObject;
}

btRigidBody * PhysicalBody::getRigidBody()
{
    return (btRigidBody*)body;
}

btCollisionObject * PhysicalBody::getCollisionObject()
{
    return body;
}
