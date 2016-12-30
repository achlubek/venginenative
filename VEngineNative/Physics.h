#pragma once

#include "PhysicalBody.h"
#include "PhysicalConstraint.h"
#include "Mesh3dInstance.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

class Physics
{
public:
    Physics();
    ~Physics();
    void simulationStep(float elapsedTime);
    void removeBody(PhysicalBody* body);
    void addBody(PhysicalBody* body);
    void removeConstraint(PhysicalConstraint* body);
    void addConstraint(PhysicalConstraint* body);
    PhysicalBody* createBody(float mass, TransformationManager* startTransform, btCollisionShape* shape);
    PhysicalBody* createBody(float mass, Mesh3dInstance* mesh, btCollisionShape* shape);
    PhysicalBody* rayCast(glm::vec3 origin, glm::vec3 direction, glm::vec3 &hitpos, glm::vec3 &hitnorm);
private:
    btDbvtBroadphase* broadphase;
    btCollisionConfiguration* collisionConf;
    std::vector<btCollisionShape*> collisionShapes;
    btCollisionDispatcher* dispatcher;
    btDiscreteDynamicsWorld* world;
    std::set<PhysicalBody*> activeBodies;
    std::vector<PhysicalBody*> addBodyQueue;
    std::vector<PhysicalBody*> removeBodyQueue;
    std::vector<PhysicalConstraint*> addConstraintQueue;
    std::vector<PhysicalConstraint*> removeConstraintQueue;
    glm::vec3 gravity = glm::vec3(0.0f, -9.87f, 0.0f);
    btRigidBody* createRigidBody(float mass, TransformationManager* transform, btCollisionShape* shape);
    void exitPhysics();
};

