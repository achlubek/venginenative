#pragma once
#include "AbsTransformable.h"
#include "TransformationManager.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

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
    TransformationManager* getTransformationManager();
    bool isEnabled();
    void readChanges();
private:
    bool enabled = false;
};

