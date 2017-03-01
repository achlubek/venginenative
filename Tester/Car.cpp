#include "stdafx.h"
#include "Car.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"

Car::Car(string definitionkey, TransformationManager * spawn)
{
    definitionReader = new INIReader(definitionkey);
    initialize(spawn->getStruct());
}

Mesh3d* Car::bodyMesh = nullptr;
Mesh3d* Car::tiresMesh = nullptr;

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

    if (acc > 0.0) {
        // accelerate
        if (speed > -2.0) {
            // forward
            float maxspeed = definitionReader->getf("max_speed_forward");
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

            float maxspeed = definitionReader->getf("max_speed_backward");
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
    if (body == nullptr || body->getRigidBody() == nullptr) return;

    wheelsAngle = angleInRadians / (1.0 + 0.1 * glm::pow(getSpeed(), 2.0));
     
    if (tyreLFCon != nullptr) {
        ((btGeneric6DofConstraint*)tyreLFCon->constraint)->setAngularLowerLimit(btVector3(-1.0, angleInRadians, 0.0));
        ((btGeneric6DofConstraint*)tyreLFCon->constraint)->setAngularUpperLimit(btVector3(-1.0, angleInRadians, 0.0));

        ((btGeneric6DofConstraint*)tyreRFCon->constraint)->setAngularLowerLimit(btVector3(-1.0, angleInRadians, 0.0));
        ((btGeneric6DofConstraint*)tyreRFCon->constraint)->setAngularUpperLimit(btVector3(-1.0, angleInRadians, 0.0));
    }
  //  auto upvec = body->transformation->getOrientation() * glm::vec3(0.0, 1.0, 0.0);
  //  glm::vec3 diff = glm::vec3(0.0, 1.0, 0.0) - upvec;
   // float dt = glm::dot(upvec, glm::vec3(0.0, 1.0, 0.0));
  //  printf("%f %f %f\n", diff.x, diff.y, diff.z);
  //  body->body->applyForce(bulletify3(diff*10.0f), bulletify3(glm::vec3(0.0, 11.0, 0.0)));
    auto angvec = body->getRigidBody()->getAngularVelocity();
    body->getRigidBody()->setAngularVelocity(btVector3(-angvec.x()*0.9f, angvec.y(), -angvec.z()*0.9f));
}

float Car::getWheelsAngle()
{
    return wheelsAngle;
}

TransformationManager * Car::getTransformation()
{
    return body->transformation;
}

float Car::getSpeed()
{
    glm::vec3 velocity = glmify3(body->getRigidBody()->getLinearVelocity());
    glm::vec3 vector_forward = body->transformation->getOrientation() * glm::vec3(0.0, 0.0, -1.0);
    float dt = -glm::dot(velocity, vector_forward);
    return glm::length(velocity) * glm::sign(dt);
}

glm::vec3 Car::getLinearVelocity()
{
    return glmify3(body->getRigidBody()->getLinearVelocity());
}

void Car::updateTyreForce(PhysicalConstraint * tyrec, bool enableMotor, float targetVelocity)
{
    float maxLimitForce = definitionReader->getf("engine_force");
    float currentLimitForce = definitionReader->getf("engine_force");
    float maxMotorForce = definitionReader->getf("engine_force");
    float loLimit = -definitionReader->getf("engine_limit");
    float hiLimit = definitionReader->getf("engine_limit");
    auto m = ((btGeneric6DofConstraint*)tyrec->constraint)->getRotationalLimitMotor(0);
    if (!enableMotor) {
        m->m_targetVelocity = 0.0f;
        m->m_enableMotor = false;
    }
    else { 
        m->m_targetVelocity = -targetVelocity;
        m->m_enableMotor = true;
    }
    m->m_maxLimitForce = maxLimitForce;
    m->m_currentLimit = currentLimitForce;
    m->m_maxMotorForce = maxMotorForce;
    m->m_loLimit = loLimit;
    m->m_hiLimit = hiLimit;
}

void Car::initialize(TransformStruct spawn)
{
    if (bodyMesh == nullptr) {
        bodyMesh = Game::instance->asset->loadMeshFile(definitionReader->gets("body_mesh"));
        tiresMesh = Game::instance->asset->loadMeshFile(definitionReader->gets("tyre_mesh")); 
        Game::instance->world->scene->addMesh3d(bodyMesh);
        Game::instance->world->scene->addMesh3d(tiresMesh);
    }
    int carsCount = bodyMesh->getInstances().size();
     
    bodyMesh->addInstance(new Mesh3dInstance(new TransformationManager(spawn.position)));

    tiresMesh->addInstance(new Mesh3dInstance(new TransformationManager(spawn.position + definitionReader->getv3("wheel_front_left"), glm::angleAxis(deg2rad(180.0f), glm::vec3(0.0, 1.0, 0.0)))));
    tiresMesh->addInstance(new Mesh3dInstance(new TransformationManager(spawn.position + definitionReader->getv3("wheel_front_right"), glm::angleAxis(deg2rad(0.0f), glm::vec3(0.0, 1.0, 0.0)))));
    tiresMesh->addInstance(new Mesh3dInstance(new TransformationManager(spawn.position + definitionReader->getv3("wheel_rear_left"), glm::angleAxis(deg2rad(180.0f), glm::vec3(0.0, 1.0, 0.0)))));
    tiresMesh->addInstance(new Mesh3dInstance(new TransformationManager(spawn.position + definitionReader->getv3("wheel_rear_right"), glm::angleAxis(deg2rad(0.0f), glm::vec3(0.0, 1.0, 0.0)))));
    

    Game::instance->invoke([&, carsCount]() {

        Game::instance->physicsInvoke([&, carsCount]() {

            auto conx = new btConvexHullShape();
            for (int i = 0; i < bodyMesh->getLodLevel(0)->info3d->vbo.size(); i += 12) {
                conx->addPoint(btVector3(bodyMesh->getLodLevel(0)->info3d->vbo[i],
                    bodyMesh->getLodLevel(0)->info3d->vbo[i + 1],
                    bodyMesh->getLodLevel(0)->info3d->vbo[i + 2]), bodyMesh->getLodLevel(0)->info3d->vbo.size() - 1 == i);
            }
            btShapeHull* hull = new btShapeHull(conx);
            btScalar margin = conx->getMargin();
            hull->buildHull(margin);
            btConvexHullShape* simplifiedConvexShape = new btConvexHullShape((btScalar*)hull->getVertexPointer(), hull->numVertices());
            body = Game::instance->world->physics->createBody(definitionReader->getf("body_mass"), bodyMesh->getInstance(carsCount), simplifiedConvexShape);
            tyreLF = Game::instance->world->physics->createBody(definitionReader->getf("tyre_mass"), tiresMesh->getInstance(carsCount*4), new btSphereShape(definitionReader->getf("tyre_radius")));
            tyreRF = Game::instance->world->physics->createBody(definitionReader->getf("tyre_mass"), tiresMesh->getInstance(carsCount*4 + 1), new btSphereShape(definitionReader->getf("tyre_radius")));
            tyreLR = Game::instance->world->physics->createBody(definitionReader->getf("tyre_mass"), tiresMesh->getInstance(carsCount*4 + 2), new btSphereShape(definitionReader->getf("tyre_radius")));
            tyreRR = Game::instance->world->physics->createBody(definitionReader->getf("tyre_mass"), tiresMesh->getInstance(carsCount*4 + 3), new btSphereShape(definitionReader->getf("tyre_radius")));

           // body->body->setDamping(0.0017, 0.006);
           // btTransform cmasstrs = body->body->getCenterOfMassTransform();
           // cmasstrs.setOrigin(cmasstrs.getOrigin() + btVector3(0.0,  2.0, 0.0));
           // body->body->setCenterOfMassTransform(cmasstrs);

            tyreLF->getRigidBody()->setFriction(definitionReader->getf("tyre_friction"));
            tyreRF->getRigidBody()->setFriction(definitionReader->getf("tyre_friction"));
            tyreLR->getRigidBody()->setFriction(definitionReader->getf("tyre_friction"));
            tyreRR->getRigidBody()->setFriction(definitionReader->getf("tyre_friction"));
            
            body->getRigidBody()->setCollisionFlags(body->getRigidBody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
            
            body->getRigidBody()->setIgnoreCollisionCheck(tyreLF->getRigidBody(), true);
            body->getRigidBody()->setIgnoreCollisionCheck(tyreRF->getRigidBody(), true);
            body->getRigidBody()->setIgnoreCollisionCheck(tyreLR->getRigidBody(), true);
            body->getRigidBody()->setIgnoreCollisionCheck(tyreRR->getRigidBody(), true);
             
            btTransform frameInA, frameInB;
            frameInA = btTransform::getIdentity();
            frameInB = btTransform::getIdentity();

            frameInA.setOrigin(bulletify3(definitionReader->getv3("wheel_front_left")));
            frameInB.setRotation(bulletifyq(glm::angleAxis(deg2rad(0.0f), glm::vec3(0.0, 1.0, 0.0))));
            auto ct1 = new btGeneric6DofSpringConstraint(*(body->getRigidBody()), *(tyreLF->getRigidBody()), frameInA, frameInB, true);
            tyreLFCon = new PhysicalConstraint(ct1, body, tyreLF);

            frameInA.setOrigin(bulletify3(definitionReader->getv3("wheel_front_right")));
            frameInB.setRotation(bulletifyq(glm::angleAxis(deg2rad(180.0f), glm::vec3(0.0, 1.0, 0.0))));
            auto ct2 = new btGeneric6DofSpringConstraint(*(body->getRigidBody()), *(tyreRF->getRigidBody()), frameInA, frameInB, true);
            tyreRFCon = new PhysicalConstraint(ct2, body, tyreRF);

            frameInA.setOrigin(bulletify3(definitionReader->getv3("wheel_rear_left")));
            frameInB.setRotation(bulletifyq(glm::angleAxis(deg2rad(0.0f), glm::vec3(0.0, 1.0, 0.0))));
            auto ct3 = new btGeneric6DofSpringConstraint(*(body->getRigidBody()), *(tyreLR->getRigidBody()), frameInA, frameInB, true);
            tyreLRCon = new PhysicalConstraint(ct3, body, tyreLR);

            frameInA.setOrigin(bulletify3(definitionReader->getv3("wheel_rear_right")));
            frameInB.setRotation(bulletifyq(glm::angleAxis(deg2rad(180.0f), glm::vec3(0.0, 1.0, 0.0))));
            auto ct4 = new btGeneric6DofSpringConstraint(*(body->getRigidBody()), *(tyreRR->getRigidBody()), frameInA, frameInB, true);
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


            float maxLimitForce = definitionReader->getf("engine_force");
            float currentLimitForce = definitionReader->getf("engine_force");
            float maxMotorForce = definitionReader->getf("engine_force");
            float loLimit = -definitionReader->getf("engine_limit");
            float hiLimit = definitionReader->getf("engine_limit");
             
            auto m = ct1->getRotationalLimitMotor(0);
            m->m_enableMotor = false;
            m->m_targetVelocity = -0;
            m->m_maxLimitForce = maxLimitForce;
            m->m_currentLimit = currentLimitForce;
            m->m_maxMotorForce = maxMotorForce;
            m->m_loLimit = loLimit;
            m->m_hiLimit = hiLimit;

            m = ct2->getRotationalLimitMotor(0);
            m->m_enableMotor = false;
            m->m_targetVelocity = -0;
            m->m_maxLimitForce = maxLimitForce;
            m->m_currentLimit = currentLimitForce;
            m->m_maxMotorForce = maxMotorForce;
            m->m_loLimit = loLimit;
            m->m_hiLimit = hiLimit;

            m = ct3->getRotationalLimitMotor(0);
            m->m_enableMotor = false;
            m->m_targetVelocity = -0;
            m->m_maxLimitForce = maxLimitForce;
            m->m_currentLimit = currentLimitForce;
            m->m_maxMotorForce = maxMotorForce;
            m->m_loLimit = loLimit;
            m->m_hiLimit = hiLimit;

            m = ct4->getRotationalLimitMotor(0);
            m->m_enableMotor = false;
            m->m_targetVelocity = -0;
            m->m_maxLimitForce = maxLimitForce;
            m->m_currentLimit = currentLimitForce;
            m->m_maxMotorForce = maxMotorForce;
            m->m_loLimit = loLimit;
            m->m_hiLimit = hiLimit;

            body->enable();
            tyreLF->enable();
            tyreRF->enable();
            tyreLR->enable();
            tyreRR->enable();

            tyreLFCon->enable();
            tyreRFCon->enable();
            tyreLRCon->enable();
            tyreRRCon->enable();


            body->getRigidBody()->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
            body->getRigidBody()->setAngularVelocity(btVector3(0.0, 0.0, 0.0));

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