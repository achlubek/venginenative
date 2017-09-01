#pragma once
#include "AbsTransformable.h"
#include "TransformationManager.h"
class PhysicalConstraint;
class btCollisionObject;
class btRigidBody;
class btCollisionShape;
class PhysicalBody : public AbsTransformable
{
public:
    PhysicalBody(btCollisionObject* rigidBody, btCollisionShape* ishape, TransformationManager* manager);
    PhysicalBody(btRigidBody* rigidBody, btCollisionShape* ishape, TransformationManager* manager);
    ~PhysicalBody();
    void applyChanges(); 
    void addConstraint(PhysicalConstraint* c);
    void removeConstraint(PhysicalConstraint* c);
    TransformationManager* getTransformationManager(); 
    void readChanges();
    bool isStatic();
    btRigidBody* getRigidBody();
    btCollisionObject* getCollisionObject();
private:
    btCollisionObject* body;
    btCollisionShape* shape;
    bool isStaticObject = false;
    std::set<PhysicalConstraint*> constraints;
};

