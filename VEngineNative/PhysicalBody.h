#pragma once
#include "AbsTransformable.h"
#include "TransformationManager.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
class PhysicalConstraint;
class PhysicalBody : public AbsTransformable
{
public:
    PhysicalBody(btCollisionObject* rigidBody, btCollisionShape* ishape, TransformationManager* manager);
    PhysicalBody(btRigidBody* rigidBody, btCollisionShape* ishape, TransformationManager* manager);
    ~PhysicalBody();
    void applyChanges();
    void disable();
    void enable();
    void addConstraint(PhysicalConstraint* c);
    void removeConstraint(PhysicalConstraint* c);
    TransformationManager* getTransformationManager();
    bool isEnabled();
    void readChanges();
    bool isStatic();
    btRigidBody* getRigidBody();
    btCollisionObject* getCollisionObject();
private:
    btCollisionObject* body;
    btCollisionShape* shape;
    bool enabled = false;
    bool isStaticObject = false;
    std::set<PhysicalConstraint*> constraints;
};

