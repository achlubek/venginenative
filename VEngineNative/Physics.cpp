#include "stdafx.h"
#include "Physics.h"


Physics::Physics()
{
    activeBodies = std::set<PhysicalBody*>();
    addBodyQueue = std::vector<PhysicalBody*>();
    removeBodyQueue = std::vector<PhysicalBody*>();
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
    world->stepSimulation(elapsedTime, 1);
    for (auto b : activeBodies)
    {
        if (b != nullptr && b->body != nullptr && b->body->getActivationState() != ISLAND_SLEEPING)
        {
            b->applyChanges();
        }
    }
    if (addBodyQueue.size() > 0)
    {
        auto lst = std::vector<PhysicalBody*>(addBodyQueue);
        addBodyQueue.clear();
        for (int i = 0; i<lst.size(); i++)
            world->addRigidBody(lst[i]->body);
    }
    if (removeBodyQueue.size() > 0)
    {
        auto lst = std::vector<PhysicalBody*>(removeBodyQueue);
        removeBodyQueue.clear();
        for (int i = 0; i<lst.size(); i++)
            world->removeRigidBody(lst[i]->body);
    }
    if (addConstraintQueue.size() > 0)
    {
        auto lst = std::vector<PhysicalConstraint*>(addConstraintQueue);
        addConstraintQueue.clear();
        for (int i = 0; i<lst.size(); i++)
            world->addConstraint(lst[i]->constraint);
    }
    if (removeConstraintQueue.size() > 0)
    {
        auto lst = std::vector<PhysicalConstraint*>(removeConstraintQueue);
        removeConstraintQueue.clear();
        for (int i = 0; i<lst.size(); i++)
            world->removeConstraint(lst[i]->constraint);
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

void Physics::removeConstraint(PhysicalConstraint * c)
{
    removeConstraintQueue.push_back(c);
}

void Physics::addConstraint(PhysicalConstraint * c)
{
    addConstraintQueue.push_back(c);
}

PhysicalBody * Physics::createBody(float mass, TransformationManager * startTransform, btCollisionShape * shape)
{
    shape->setLocalScaling(btVector3(startTransform->size.x, startTransform->size.y, startTransform->size.z));
    auto rb = createRigidBody(mass, startTransform, shape);
    auto pb = new PhysicalBody(rb, shape, startTransform);
    return pb;
}

PhysicalBody * Physics::createBody(float mass, Mesh3dInstance * mesh, btCollisionShape * shape)
{
    return createBody(mass, mesh->transformation, shape);
}

btVector3 vbulletify3(glm::vec3 v) {
    return btVector3(v.x, v.y, v.z);
}

glm::vec3 vglmify3(btVector3 v) {
    return glm::vec3(v.x(), v.y(), v.z());
}

btQuaternion vbulletifyq(glm::quat v) {
    return btQuaternion(v.x, v.y, v.z, v.w);
}

glm::quat vglmifyq(btQuaternion v) {
    return glm::quat(v.x(), v.y(), v.z(), v.w());
}

PhysicalBody * Physics::rayCast(glm::vec3 origin, glm::vec3 direction, glm::vec3 &hitpos, glm::vec3 &hitnorm)
{
    auto start = vbulletify3(origin);
    auto end = vbulletify3(origin + direction * 10000.0f);
    btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);

    world->rayTest(start, end, rayCallback);

    if (rayCallback.hasHit()) {
        hitpos = vglmify3(rayCallback.m_hitPointWorld);
        hitnorm = vglmify3(rayCallback.m_hitNormalWorld);
        auto rigid = (btRigidBody*)rayCallback.m_collisionObject; // risky

        for (auto b : activeBodies)
        {
            if (b->body == rigid) return b;
        }

    }
    return nullptr;
}

btRigidBody * Physics::createRigidBody(float mass, TransformationManager* transform, btCollisionShape * shape)
{
    bool isDynamic = (mass != 0.0f);

    btVector3 localInertia = btVector3(0.0, 0.0, 0.0);
    if (isDynamic)
        shape->calculateLocalInertia(mass, localInertia);

    auto q = btQuaternion(transform->orientation.x, transform->orientation.y,
        transform->orientation.z, transform->orientation.w);
    auto v = btVector3(transform->position.x, transform->position.y, transform->position.z);

    auto bt = btTransform();
    bt.setOrigin(v);
    bt.setRotation(q);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(bt);

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
