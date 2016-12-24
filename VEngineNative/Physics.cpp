#include "stdafx.h"
#include "Physics.h"


Physics::Physics()
{
    activeBodies = std::set<PhysicalBody*>();
    addBodyQueue = std::vector<PhysicalBody*>();
    removeBodyQueue = std::vector<PhysicalBody*>();

    collisionConf = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConf);
    broadphase = new btDbvtBroadphase();
    auto w = new btDiscreteDynamicsWorld(dispatcher, broadphase, new btSequentialImpulseConstraintSolver(), collisionConf);
    w->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
    world = w;
}


Physics::~Physics()
{
}

void Physics::simulationStep(float elapsedTime)
{
    if (activeBodies.size() == 0 || world == nullptr)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return;
    }
    if (addBodyQueue.size() > 0)
    {
        auto lst = std::vector<PhysicalBody*>(addBodyQueue);
        addBodyQueue.clear();
        for(int i=0;i<lst.size();i++)
            world->addRigidBody(lst[i]->body);
    }
    if (removeBodyQueue.size() > 0)
    { 
        auto lst = std::vector<PhysicalBody*>(removeBodyQueue);
        removeBodyQueue.clear();
        for (int i = 0; i<lst.size(); i++)
            world->removeRigidBody(lst[i]->body);
    }
    world->stepSimulation(elapsedTime);
    for (auto b : activeBodies)
    {
        if (b != nullptr && b->body != nullptr && b->body->getActivationState() != ISLAND_SLEEPING)
        {
            b->applyChanges();
        }
    }
}

void Physics::removeBody(PhysicalBody * body)
{
    removeBodyQueue.push_back(body);
    activeBodies.erase(body);
}

void Physics::addBody(PhysicalBody * body)
{
    addBodyQueue.push_back(body);
    activeBodies.emplace(body);
}

PhysicalBody * Physics::createBody(float mass, TransformationManager * startTransform, btCollisionShape * shape)
{
    shape->setLocalScaling(btVector3(startTransform->size.x, startTransform->size.y, startTransform->size.z));
    auto rb = createRigidBody(mass, shape);
    auto pb = new PhysicalBody(rb, shape, startTransform);
    pb->readChanges();
    return pb;
}

PhysicalBody * Physics::createBody(float mass, Mesh3dInstance * mesh, btCollisionShape * shape)
{
    return createBody(mass, mesh->transformation, shape);
}

btRigidBody * Physics::createRigidBody(float mass,  btCollisionShape * shape)
{
    bool isDynamic = (mass != 0.0f);

    btVector3 localInertia = btVector3(0.0, 0.0, 0.0);
    if (isDynamic)
        shape->calculateLocalInertia(mass, localInertia);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(btTransform());

    btRigidBody::btRigidBodyConstructionInfo* rbInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, myMotionState, shape, localInertia);
    btRigidBody* body = new btRigidBody(*rbInfo);


    body->setSleepingThresholds(0.0f, 0.0f);
    body->setContactProcessingThreshold(0.0f);
    body->setCcdMotionThreshold(0.0f);
    
    return body;
}

void Physics::exitPhysics()
{
}
