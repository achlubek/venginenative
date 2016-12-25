#pragma once
#include "AbsTransformable.h"
#include "TransformationManager.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
class PhysicalConstraint;
class PhysicalBody : public AbsTransformable
{
public:
    PhysicalBody(btRigidBody* rigidBody, btCollisionShape* ishape, TransformationManager* manager);
    ~PhysicalBody();
    btRigidBody* body;
    btCollisionShape* shape;
    void applyChanges();
    void disable();
    void enable();
    void addConstraint(PhysicalConstraint* c);
    void removeConstraint(PhysicalConstraint* c);
    TransformationManager* getTransformationManager();
    bool isEnabled();
    void readChanges();
private:
    bool enabled = false;
    std::set<PhysicalConstraint*> constraints;
};

