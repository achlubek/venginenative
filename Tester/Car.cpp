#include "stdafx.h"
#include "Car.h"


Car::Car()
{
    initialize();
}


Car::~Car()
{
}

void Car::draw()
{
}

void Car::setAcceleration(float acc)
{
    if (acc < 0.0) acc *= 0.1;
    if (!initialized) return;
    acceleration = acceleration * 0.999f + 0.001f * acc;
    if (tyreLFCon != nullptr) {
        float maxspeed = 400.0f;
        auto m = ((btGeneric6DofConstraint*)tyreLFCon->constraint)->getRotationalLimitMotor(0);
        m->m_targetVelocity = -acceleration * maxspeed;
        m->m_maxLimitForce = 35.0f;
        m->m_currentLimit = 5.0f;
        m->m_maxMotorForce = 35.0f;
        m->m_enableMotor = acc > 0.01f || acc < 0.01f;
        m->m_loLimit = -5.0f;
        m->m_hiLimit = 5.0f;
        m = ((btGeneric6DofConstraint*)tyreRFCon->constraint)->getRotationalLimitMotor(0);
        m->m_targetVelocity = -acceleration * maxspeed;
        m->m_maxLimitForce = 35.0f;
        m->m_currentLimit = 5.0f;
        m->m_maxMotorForce = 35.0f;
        m->m_enableMotor = acc > 0.01f || acc < 0.01f;
        m->m_loLimit = -5.0f;
        m->m_hiLimit = 5.0f;
    }
}

void Car::setWheelsAngle(float angleInRadians)
{

    if (!initialized) return;
    if (body == nullptr || body->body == nullptr) return;
    angleInRadians /= 1.0f + body->body->getLinearVelocity().length2() * 0.02f;
    if (tyreLFCon != nullptr) {
        ((btGeneric6DofConstraint*)tyreLFCon->constraint)->setAngularLowerLimit(btVector3(-1.0, angleInRadians, 0.0));
        ((btGeneric6DofConstraint*)tyreLFCon->constraint)->setAngularUpperLimit(btVector3(-1.0, angleInRadians, 0.0));

        ((btGeneric6DofConstraint*)tyreRFCon->constraint)->setAngularLowerLimit(btVector3(-1.0, angleInRadians, 0.0));
        ((btGeneric6DofConstraint*)tyreRFCon->constraint)->setAngularUpperLimit(btVector3(-1.0, angleInRadians, 0.0));
    }

    auto v1 = glm::normalize(glmify3(body->body->getLinearVelocity()));
    auto v2 = glm::normalize(glmifyq(body->body->getOrientation()) * glm::vec3(0.0, 0.0, -1.0));
    body->body->applyCentralForce(bulletify3((v2 - v1) * 9120.0f));
}

TransformationManager * Car::getTransformation()
{
    return body->transformation;
}

void Car::initialize()
{

    Game::instance->invoke([&]() {
        bodyMesh = Game::instance->asset->loadMeshFile("fiesta_body.mesh3d");
        tiresMesh = Game::instance->asset->loadMeshFile("fiesta_tyre.mesh3d");
        
        glm::vec3 frontaxis = glm::vec3(0.0f, -0.538f, 1.13108f);
        glm::vec3 rearaxis = glm::vec3(0.0f, -0.538f, -1.2333f);
        glm::vec3 wheelspacing = glm::vec3(0.70968f, 0.0f, 0.0f);

        bodyMesh->addInstance(new Mesh3dInstance(new TransformationManager(glm::vec3(0.0, 6.0, 0.0))));

        tiresMesh->addInstance(new Mesh3dInstance(new TransformationManager(glm::vec3(0.0, 6.0, 0.0) + frontaxis + wheelspacing, glm::angleAxis(deg2rad(180.0f), glm::vec3(0.0, 1.0, 0.0)))));
        tiresMesh->addInstance(new Mesh3dInstance(new TransformationManager(glm::vec3(0.0, 6.0, 0.0) + frontaxis - wheelspacing, glm::angleAxis(deg2rad(0.0f), glm::vec3(0.0, 1.0, 0.0)))));
        tiresMesh->addInstance(new Mesh3dInstance(new TransformationManager(glm::vec3(0.0, 6.0, 0.0) + rearaxis + wheelspacing , glm::angleAxis(deg2rad(180.0f), glm::vec3(0.0, 1.0, 0.0)))));
        tiresMesh->addInstance(new Mesh3dInstance(new TransformationManager(glm::vec3(0.0, 6.0, 0.0) + rearaxis - wheelspacing , glm::angleAxis(deg2rad(0.0f), glm::vec3(0.0, 1.0, 0.0)))));
        Game::instance->world->scene->addMesh(bodyMesh);
        Game::instance->world->scene->addMesh(tiresMesh);
        Game::instance->physicsInvoke([&]() {
            glm::vec3 frontaxis = glm::vec3(0.0f, -0.538f, 1.13108f);
            glm::vec3 rearaxis = glm::vec3(0.0f, -0.538f, -1.2333f);
            glm::vec3 wheelspacing = glm::vec3(0.70968f, 0.0f, 0.0f);

            body = Game::instance->world->physics->createBody(1200.0, bodyMesh->getInstance(0), new btBoxShape(btVector3(1.0f, 0.45f, 3.2f)));
            tyreLF = Game::instance->world->physics->createBody(8.0f, tiresMesh->getInstance(0), new btSphereShape(0.275f));
            tyreRF = Game::instance->world->physics->createBody(8.0f, tiresMesh->getInstance(1), new btSphereShape(0.275f));
            tyreLR = Game::instance->world->physics->createBody(8.0f, tiresMesh->getInstance(2), new btSphereShape(0.275f));
            tyreRR = Game::instance->world->physics->createBody(8.0f, tiresMesh->getInstance(3), new btSphereShape(0.275f));

            body->body->setDamping(0.17, 0.06);

            tyreLF->body->setFriction(1410.0);
            tyreRF->body->setFriction(1410.0);
            tyreLR->body->setFriction(1410.0);
            tyreRR->body->setFriction(1410.0);
            body->body->setIgnoreCollisionCheck(tyreLF->body, true);
            body->body->setIgnoreCollisionCheck(tyreRF->body, true);
            body->body->setIgnoreCollisionCheck(tyreLR->body, true);
            body->body->setIgnoreCollisionCheck(tyreRR->body, true);

            auto v4 = frontaxis + wheelspacing;
            btTransform frameInA, frameInB;
            frameInA = btTransform::getIdentity();
            frameInB = btTransform::getIdentity();

            frameInA.setOrigin(bulletify3(frontaxis + wheelspacing));
            frameInB.setRotation(bulletifyq(glm::angleAxis(deg2rad(0.0f), glm::vec3(0.0, 1.0, 0.0))));
            auto ct1 = new btGeneric6DofSpringConstraint(*(body->body), *(tyreLF->body), frameInA, frameInB, true);
            ct1->setAngularLowerLimit(btVector3(0.0, 10.0, 0.0));
            tyreLFCon = new PhysicalConstraint(ct1, body, tyreLF);

            frameInA.setOrigin(bulletify3(frontaxis - wheelspacing));
            frameInB.setRotation(bulletifyq(glm::angleAxis(deg2rad(180.0f), glm::vec3(0.0, 1.0, 0.0))));
            auto ct2 = new btGeneric6DofSpringConstraint(*(body->body), *(tyreRF->body), frameInA, frameInB, true);
            tyreRFCon = new PhysicalConstraint(ct2, body, tyreRF);

            frameInA.setOrigin(bulletify3(rearaxis + wheelspacing));
            frameInB.setRotation(bulletifyq(glm::angleAxis(deg2rad(0.0f), glm::vec3(0.0, 1.0, 0.0))));
            auto ct3 = new btGeneric6DofSpringConstraint(*(body->body), *(tyreLR->body), frameInA, frameInB, true);
            tyreLRCon = new PhysicalConstraint(ct3, body, tyreLR);

            frameInA.setOrigin(bulletify3(rearaxis - wheelspacing));
            frameInB.setRotation(bulletifyq(glm::angleAxis(deg2rad(180.0f), glm::vec3(0.0, 1.0, 0.0))));
            auto ct4 = new btGeneric6DofSpringConstraint(*(body->body), *(tyreRR->body), frameInA, frameInB, true);
            tyreRRCon = new PhysicalConstraint(ct4, body, tyreRR);

            auto x = std::vector<btGeneric6DofSpringConstraint*>{ ct1, ct2, ct3, ct4 };
            for (int i = 0; i < x.size(); i++) {
                float y = (i == 0 || i == 1) ? -0.0f : 0.0f;
                x[i]->setAngularLowerLimit(btVector3(-1.0, y, 0.0));
                x[i]->setAngularUpperLimit(btVector3(-1.0, y, 0.0));
                x[i]->enableSpring(0, false);
                x[i]->enableSpring(1, false);
                x[i]->enableSpring(2, false);
                x[i]->setDamping(0, 0.02f);
                x[i]->setDamping(1, 0.02f);
                x[i]->setDamping(2, 0.02f);
                x[i]->setLinearLowerLimit(btVector3(0.0, -0.1, 0.0));
                x[i]->setLinearUpperLimit(btVector3(0.0, 0.0, 0.0));
                x[i]->setBreakingImpulseThreshold(0.01f);
            }

            tyreLFCon->constraint->setBreakingImpulseThreshold(99990.0f);
            tyreRFCon->constraint->setBreakingImpulseThreshold(99990.0f);
            tyreLRCon->constraint->setBreakingImpulseThreshold(99990.0f);
            tyreRRCon->constraint->setBreakingImpulseThreshold(99990.0f);

            body->enable();
            tyreLF->enable();
            tyreRF->enable();
            tyreLR->enable();
            tyreRR->enable();

            tyreLFCon->enable();
            tyreRFCon->enable();
            tyreLRCon->enable();
            tyreRRCon->enable();
           // body->body->setLinearVelocity(btVector3(3.0, 15.0, 0.0));
            body->body->setAngularVelocity(btVector3(0.0, 0.0, 1));
            auto m = ct1->getRotationalLimitMotor(0);
            m->m_targetVelocity = -0;
            m->m_maxLimitForce = 50.0f;
            m->m_currentLimit = 50.0f;
            m->m_maxMotorForce = 50.0f;
            m->m_enableMotor = true;
            m->m_loLimit = -5.0f;
            m->m_hiLimit = 5.0f;
            m = ct2->getRotationalLimitMotor(0);
            m->m_targetVelocity = -0;
            m->m_maxLimitForce = 50.0f;
            m->m_currentLimit = 50.0f;
            m->m_maxMotorForce = 50.0f;
            m->m_enableMotor = true;
            m->m_loLimit = -5.0f;
            m->m_hiLimit = 5.0f;

            m = ct3->getRotationalLimitMotor(0);
            m->m_targetVelocity = 0;
            m->m_maxLimitForce = 9999.0f;
            m->m_currentLimit = 9999.0f;
            m->m_maxMotorForce = 9999.0f;
            m->m_enableMotor = false;
            m->m_loLimit = -50.0f;
            m->m_hiLimit = 50.0f;
            m = ct4->getRotationalLimitMotor(0);
            m->m_targetVelocity = 0;
            m->m_maxLimitForce = 9999.0f;
            m->m_currentLimit = 9999.0f;
            m->m_maxMotorForce = 9999.0f;
            m->m_enableMotor = false;
            m->m_loLimit = -50.0f;
            m->m_hiLimit = 50.0f;
            

            Game::instance->invoke([&]() {
                initialized = true;
            });
        });
    });
}

float Car::getAirTorque()
{
    return 0.0f;
}