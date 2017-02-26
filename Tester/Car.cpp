#include "stdafx.h"
#include "Car.h"


Car::Car(TransformationManager * spawn)
{
    initialize(spawn->getStruct());
}


Car::~Car()
{
}

void Car::draw()
{
}

void Car::setAcceleration(float acc)
{
    // 5 situations:
    // Going forward + accelerate (apply motor forward)
    // Going forward + deccelerate (apply brakes locking tires)
    // Going backward + accelerate (apply brakes locking tires)
    // Going backward + deccelerate (apply motor backward)
    // No acceleration/decceleration (release motors)

    if (!initialized) return;
    if (tyreLFCon == nullptr) return;


    if (acc > -0.05 && acc < 0.05) {
        updateTyreForce(tyreLFCon, false);
        updateTyreForce(tyreRFCon, false);
        updateTyreForce(tyreLRCon, false);
        updateTyreForce(tyreRRCon, false);
        return;
    }

    float speed = getSpeed();
    float acceleration = acc;

    float maxLimitForce = 15.0f;
    float currentLimitForce = 5.0f;
    float maxMotorForce = 15.0f;
    float loLimit = 39.0f;
    float hiLimit = 39.0f;

    if (acc > 0.0) {
        // accelerate
        if (speed > -2.0) {
            // forward
            float maxspeed = 900.0f;
            updateTyreForce(tyreLFCon, true, acceleration * maxspeed);
            updateTyreForce(tyreRFCon, true, acceleration * maxspeed);

            updateTyreForce(tyreLRCon, false);
            updateTyreForce(tyreRRCon, false);
        }
        else {
            // backward

            updateTyreForce(tyreLFCon, true, 0.0f);
            updateTyreForce(tyreRFCon, true, 0.0f);
            updateTyreForce(tyreLRCon, true, 0.0f);
            updateTyreForce(tyreRRCon, true, 0.0f);
        }
    }
    else {
        // deccelerate
        if (speed > 2.0) {
            // forward

            updateTyreForce(tyreLFCon, true, 0.0f);
            updateTyreForce(tyreRFCon, true, 0.0f);
            updateTyreForce(tyreLRCon, true, 0.0f);
            updateTyreForce(tyreRRCon, true, 0.0f);
        }
        else {
            // backward

            float maxspeed = 210.0f;
            updateTyreForce(tyreLFCon, true, acceleration * maxspeed);
            updateTyreForce(tyreRFCon, true, acceleration * maxspeed);

            updateTyreForce(tyreLRCon, false);
            updateTyreForce(tyreRRCon, false);
        }
    }
}

void Car::setWheelsAngle(float angleInRadians)
{

    if (!initialized) return;
    if (body == nullptr || body->body == nullptr) return;
    
    float speed = glm::length(glmify3(body->body->getLinearVelocity()));
    angleInRadians /= 1.0f + speed*speed * 0.01f;
    if (tyreLFCon != nullptr) {
        ((btGeneric6DofConstraint*)tyreLFCon->constraint)->setAngularLowerLimit(btVector3(-1.0, angleInRadians, 0.0));
        ((btGeneric6DofConstraint*)tyreLFCon->constraint)->setAngularUpperLimit(btVector3(-1.0, angleInRadians, 0.0));

        ((btGeneric6DofConstraint*)tyreRFCon->constraint)->setAngularLowerLimit(btVector3(-1.0, angleInRadians, 0.0));
        ((btGeneric6DofConstraint*)tyreRFCon->constraint)->setAngularUpperLimit(btVector3(-1.0, angleInRadians, 0.0));
    }
    
}

TransformationManager * Car::getTransformation()
{
    return body->transformation;
}

float Car::getSpeed()
{
    glm::vec3 velocity = glmify3(body->body->getLinearVelocity());
    glm::vec3 vector_forward = body->transformation->getOrientation() * glm::vec3(0.0, 0.0, -1.0);
    float dt = -glm::dot(velocity, vector_forward);
    return glm::length(velocity) * glm::sign(dt);
}

void Car::updateTyreForce(PhysicalConstraint * tyrec, bool enableMotor, float targetVelocity)
{
    auto m = ((btGeneric6DofConstraint*)tyrec->constraint)->getRotationalLimitMotor(0);
    if (!enableMotor) {
        m->m_enableMotor = false;
        m->m_targetVelocity = 990.0f;
        m->m_maxLimitForce = 990.0f;
        m->m_currentLimit = 990.0f;
        m->m_maxMotorForce = 990.0f;
        m->m_loLimit = -990.0f;
        m->m_hiLimit = 990.0f;
    }
    else {

        float maxLimitForce = 15.0f;
        float currentLimitForce = 5.0f;
        float maxMotorForce = 15.0f;
        float loLimit = 39.0f;
        float hiLimit = 39.0f;
         
        auto m = ((btGeneric6DofConstraint*)tyrec->constraint)->getRotationalLimitMotor(0);
        m->m_enableMotor = true;
        m->m_targetVelocity = -targetVelocity;
        m->m_maxLimitForce = maxLimitForce;
        m->m_currentLimit = currentLimitForce;
        m->m_maxMotorForce = maxMotorForce;
        m->m_loLimit = -loLimit;
        m->m_hiLimit = hiLimit;
    }
}

void Car::initialize(TransformStruct spawn)
{
    bodyMesh = Game::instance->asset->loadMeshFile("fiesta_body.mesh3d");
    tiresMesh = Game::instance->asset->loadMeshFile("fiesta_tyre.mesh3d");

    glm::vec3 frontaxis = glm::vec3(0.0f, -0.538f, 1.13108f);
    glm::vec3 rearaxis = glm::vec3(0.0f, -0.538f, -1.2333f);
    glm::vec3 wheelspacing = glm::vec3(0.70968f, 0.0f, 0.0f);

    bodyMesh->addInstance(new Mesh3dInstance(new TransformationManager(spawn.position)));

    tiresMesh->addInstance(new Mesh3dInstance(new TransformationManager(spawn.position + frontaxis + wheelspacing, glm::angleAxis(deg2rad(180.0f), glm::vec3(0.0, 1.0, 0.0)))));
    tiresMesh->addInstance(new Mesh3dInstance(new TransformationManager(spawn.position + frontaxis - wheelspacing, glm::angleAxis(deg2rad(0.0f), glm::vec3(0.0, 1.0, 0.0)))));
    tiresMesh->addInstance(new Mesh3dInstance(new TransformationManager(spawn.position + rearaxis + wheelspacing, glm::angleAxis(deg2rad(180.0f), glm::vec3(0.0, 1.0, 0.0)))));
    tiresMesh->addInstance(new Mesh3dInstance(new TransformationManager(spawn.position + rearaxis - wheelspacing, glm::angleAxis(deg2rad(0.0f), glm::vec3(0.0, 1.0, 0.0)))));
    Game::instance->world->scene->addMesh3d(bodyMesh);
    Game::instance->world->scene->addMesh3d(tiresMesh);

    Game::instance->invoke([&]() {

        Game::instance->physicsInvoke([&]() {
            glm::vec3 frontaxis = glm::vec3(0.0f, -0.538f, 1.13108f);
            glm::vec3 rearaxis = glm::vec3(0.0f, -0.538f, -1.2333f);
            glm::vec3 wheelspacing = glm::vec3(0.70968f, 0.0f, 0.0f);

            body = Game::instance->world->physics->createBody(600.0, bodyMesh->getInstance(0), new btBoxShape(btVector3(0.1f, 0.135f, 0.2f)));
            tyreLF = Game::instance->world->physics->createBody(278.0f, tiresMesh->getInstance(0), new btSphereShape(0.275f));
            tyreRF = Game::instance->world->physics->createBody(278.0f, tiresMesh->getInstance(1), new btSphereShape(0.275f));
            tyreLR = Game::instance->world->physics->createBody(278.0f, tiresMesh->getInstance(2), new btSphereShape(0.275f));
            tyreRR = Game::instance->world->physics->createBody(278.0f, tiresMesh->getInstance(3), new btSphereShape(0.275f));

            body->body->setDamping(0.0017, 0.006);

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
            //body->body->setAngularVelocity(btVector3(0.0, 0.0, 0.0));

            float maxLimitForce = 35.0f;
            float currentLimitForce = 5.0f;
            float maxMotorForce = 35.0f;
            float loLimit = 5.0f;
            float hiLimit = 5.0f;

            auto m = ct1->getRotationalLimitMotor(0);
            m->m_targetVelocity = -0;
            m->m_maxLimitForce = maxLimitForce;
            m->m_currentLimit = currentLimitForce;
            m->m_maxMotorForce = maxMotorForce;
            m->m_enableMotor = false;
            m->m_loLimit = -loLimit;
            m->m_hiLimit = hiLimit;

            m = ct2->getRotationalLimitMotor(0);
            m->m_maxLimitForce = maxLimitForce;
            m->m_currentLimit = currentLimitForce;
            m->m_maxMotorForce = maxMotorForce;
            m->m_enableMotor = false;
            m->m_loLimit = -loLimit;
            m->m_hiLimit = hiLimit;

            m = ct3->getRotationalLimitMotor(0);
            m->m_maxLimitForce = maxLimitForce;
            m->m_currentLimit = currentLimitForce;
            m->m_maxMotorForce = maxMotorForce;
            m->m_enableMotor = false;
            m->m_loLimit = -loLimit;
            m->m_hiLimit = hiLimit;

            m = ct4->getRotationalLimitMotor(0);
            m->m_maxLimitForce = maxLimitForce;
            m->m_currentLimit = currentLimitForce;
            m->m_maxMotorForce = maxMotorForce;
            m->m_enableMotor = false;
            m->m_loLimit = -loLimit;
            m->m_hiLimit = hiLimit;


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