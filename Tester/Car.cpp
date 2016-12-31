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
}

void Car::setWheelsAngle(float angleInRadians)
{
}

void Car::initialize()
{

    Game::instance->invoke([&]() {
        bodyMesh = Game::instance->asset->loadMeshFile("fiesta_body.mesh3d");
        tiresMesh = Game::instance->asset->loadMeshFile("fiesta_tyre.mesh3d");

        bodyMesh->alwaysUpdateBuffer = true;
        tiresMesh->alwaysUpdateBuffer = true;

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

            body = Game::instance->world->physics->createBody(800.0f, bodyMesh->getInstance(0), new btBoxShape(btVector3(0.5f, 0.5f, 0.5f)));
            tyreLF = Game::instance->world->physics->createBody(2.0f, tiresMesh->getInstance(0), new btSphereShape(0.275f));
            tyreRF = Game::instance->world->physics->createBody(2.0f, tiresMesh->getInstance(1), new btSphereShape(0.275f));
            tyreLR = Game::instance->world->physics->createBody(2.0f, tiresMesh->getInstance(2), new btSphereShape(0.275f));
            tyreRR = Game::instance->world->physics->createBody(2.0f, tiresMesh->getInstance(3), new btSphereShape(0.275f));
       //     body->body->setIgnoreCollisionCheck(tyreLF->body, true);
      //      body->body->setIgnoreCollisionCheck(tyreRF->body, true);
      //      body->body->setIgnoreCollisionCheck(tyreLR->body, true);
      //      body->body->setIgnoreCollisionCheck(tyreRR->body, true);

            auto v4 = frontaxis + wheelspacing;
            btTransform frameInA, frameInB;
            frameInA = btTransform::getIdentity();

            frameInA.setOrigin(bulletify3(frontaxis + wheelspacing));
            auto ct1 = new btGeneric6DofConstraint(*(body->body), *(tyreLF->body), frameInA, btTransform::getIdentity(), true);
            ct1->setAngularLowerLimit(btVector3(0.0, 10.0, 0.0));
            tyreLFCon = new PhysicalConstraint(ct1, body, tyreLF);

            frameInA.setOrigin(bulletify3(frontaxis - wheelspacing));
            auto ct2 = new btGeneric6DofConstraint(*(body->body), *(tyreRF->body), frameInA, btTransform::getIdentity(), true);
            tyreRFCon = new PhysicalConstraint(ct2, body, tyreRF);

            frameInA.setOrigin(bulletify3(rearaxis + wheelspacing));
            auto ct3 = new btGeneric6DofConstraint(*(body->body), *(tyreLR->body), frameInA, btTransform::getIdentity(), true);
            tyreLRCon = new PhysicalConstraint(ct3, body, tyreLR);

            frameInA.setOrigin(bulletify3(rearaxis - wheelspacing));
            auto ct4 = new btGeneric6DofConstraint(*(body->body), *(tyreRR->body), frameInA, btTransform::getIdentity(), true);
            tyreRRCon = new PhysicalConstraint(ct4, body, tyreRR);

            auto x = std::vector<btGeneric6DofConstraint*>{ ct1, ct2, ct3, ct4 };
            for (int i = 0; i < x.size(); i++) {
                x[i]->setAngularLowerLimit(btVector3(0.0, 0.0, 1.0));
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
            body->body->setLinearVelocity(btVector3(3.0, 15.0, 0.0));
            body->body->setAngularVelocity(btVector3(1.0, 1.0, 0.0));

        });
    });
}

float Car::getAirTorque()
{
    return 0.0f;
}
