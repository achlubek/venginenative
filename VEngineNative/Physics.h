#pragma once

#include "PhysicalBody.h"
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
    PhysicalBody* createBody(float mass, TransformationManager* startTransform, btCollisionShape* shape);
    PhysicalBody* createBody(float mass, Mesh3dInstance* mesh, btCollisionShape* shape);
private:
    btDbvtBroadphase* broadphase;
    btCollisionConfiguration* collisionConf;
    std::vector<btCollisionShape*> collisionShapes;
    btCollisionDispatcher* dispatcher;
    btDiscreteDynamicsWorld* world;
    std::set<PhysicalBody*> activeBodies;
    std::vector<PhysicalBody*> addBodyQueue;
    std::vector<PhysicalBody*> removeBodyQueue;
    glm::vec3 gravity = glm::vec3(0.0f, -9.87f, 0.0f);
    btRigidBody* createRigidBody(float mass, btCollisionShape* shape);
    void exitPhysics();
};

