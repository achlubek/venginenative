#include "stdafx.h"
#include "SimpleWalker.h"
#include "Mesh3d.h"
#include "Physics.h"
#include "PhysicalBody.h"



SimpleWalker::SimpleWalker(Physics * iphysics, TransformationManager * spawn)
    : physics(iphysics)
{
    walkerScene = new Scene(spawn);  
    auto material = new Material();
    mesh_left_arm_up    = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_arm.raw"), material);
    mesh_left_arm_down  = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_hand.raw"), material);
    mesh_left_leg_up    = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_hip.raw"), material);
    mesh_left_leg_down  = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_leg.raw"), material);

    mesh_right_arm_up = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_arm.raw"), material);
    mesh_right_arm_down = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_hand.raw"), material);
    mesh_right_leg_up = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_hip.raw"), material);
    mesh_right_leg_down = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_leg.raw"), material);


    mesh_body = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_body.raw"), material);
    mesh_head = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_head.raw"), material);
     

    debug_marker = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("icos.raw"), material);

    for (int i = 0; i < debug_marker->getInstances().size(); i++) {
  //      debug_marker->getInstance(i)->transformation->setSize(glm::vec3(0.05));
    }
    
    walkerScene->addMesh3d(mesh_left_arm_up);
    walkerScene->addMesh3d(mesh_left_arm_down);
    walkerScene->addMesh3d(mesh_right_arm_up);
    walkerScene->addMesh3d(mesh_right_arm_down);
    walkerScene->addMesh3d(mesh_left_leg_up);
    walkerScene->addMesh3d(mesh_left_leg_down);
    walkerScene->addMesh3d(mesh_right_leg_up);
    walkerScene->addMesh3d(mesh_right_leg_down);
    walkerScene->addMesh3d(mesh_body);
    walkerScene->addMesh3d(mesh_head);
   // walkerScene->addMesh3d(debug_marker);
    

    physicalEntity = physics->createBody(2.1, new TransformationManager(spawn->getPosition()), new btSphereShape(2.8f));
    physics->addBody(physicalEntity);
}

SimpleWalker::~SimpleWalker()
{
}

float makemodulate(float speed, float shift, float step) {
    float s1 = 0.0;
    float modulation = glm::fract(speed * Application::instance->time + shift);
    if (modulation >= 0.0 && modulation < step) {
        s1 = modulation / step;
    }
    else if (modulation >= step && modulation < 1.0) {
        s1 = 1.0 - (modulation - step) / (1.0 - step);
    }
    s1 *= s1;
    s1 = s1 * 2.0 - 1.0;
    return s1;
}
float makemodulate2(float speed, float shift) {
    float s1 = 0.0;
    float modulation = glm::fract(speed * Application::instance->time + shift);
    if (modulation >= 0.0 && modulation < 0.5) {
        s1 = modulation / 0.5;
        s1 = sqrt(sqrt(s1));
    }
    else if (modulation >= 0.5 && modulation < 1.0) {
        s1 = 1.0 - ((modulation - 0.5) / 0.5);
        s1 = s1 * s1 * s1;
    }
    return s1 * 2.0 - 1.0;
}
glm::vec3 fromvec4(glm::vec4 a) {
    return glm::vec3(a.x, a.y, a.z);
}
glm::mat4 getThatSpecialTransform(TransformationManager* m) {
    glm::mat4 rotmat = glm::mat4_cast(glm::inverse(m->getOrientation()));
    glm::mat4 transmat = glm::translate(glm::mat4(1), m->getPosition());
    glm::mat4 scalemat = glm::scale(glm::mat4(1), m->getSize());
    return transmat * rotmat * scalemat;
}
void SimpleWalker::update()
{
    auto trans = getThatSpecialTransform(mesh_body->getInstance(0)->transformation);
    trans = glm::scale(trans, 1.0f / mesh_body->getInstance(0)->transformation->getSize());
#define transformvector(a) fromvec4(trans*glm::vec4(a,1.0))
    float speed = 1.0;/*
    if (state == SIMPLEWALKER_STATE_IDLE) {
        physicalEntity->getRigidBody()->setLinearVelocity(physicalEntity->getRigidBody()->getLinearVelocity() * 0.7f);
        left_shoulder->setPosition(glm::vec3(0.3, 0.2, 0.0));
        right_shoulder->setPosition(glm::vec3(-0.3, 0.2, 0.0));

        left_elbow->setPosition(glm::vec3(0.4, -0.5, 0.1));
        right_elbow->setPosition(glm::vec3(-0.4, -0.5, 0.0));

        left_hand->setPosition(glm::vec3(0.4, -1.5, 0.1));
        right_hand->setPosition(glm::vec3(-0.4, -1.5, 0.0));

        left_hip->setPosition(glm::vec3(0.2, -1.0, 0.0));
        right_hip->setPosition(glm::vec3(-0.2, -1.0, 0.0));

        left_knee->setPosition(glm::vec3(0.2, -1.8, 0.0));
        right_knee->setPosition(glm::vec3(-0.2, -1.8, 0.1));

        left_foot->setPosition(glm::vec3(0.2, -2.8, 0.0));
        right_foot->setPosition(glm::vec3(-0.2, -2.8, 0.1));
    }
    if (state == SIMPLEWALKER_STATE_WALKING) {

        physicalEntity->getRigidBody()->setLinearVelocity(bulletify3(0.6f * glm::normalize(target - walkerScene->transformation->getPosition())));

        float modulation = glm::fract(Application::instance->time);
        float modulation_halfforward = glm::fract(0.5 + Application::instance->time);
        float modulation_quadforward = glm::fract(0.75 + Application::instance->time);
        float avance = 0.85;
        float s1 = makemodulate(speed, 0.0, 0.2); // knee
        float s3 = makemodulate(speed, 0.5, 0.2); // knee
        float s2 = makemodulate(speed, avance, 0.2); // foot
        float s4 = makemodulate(speed, 0.5f + avance - 1.0f, 0.2); // foot

        float avance2 = 0.75;
        float r1 = makemodulate2(speed, 0.0);
        float r3 = makemodulate2(speed, 0.5);
        float r2 = makemodulate2(speed, avance2);
        float r4 = makemodulate2(speed, 0.5f + avance2 - 1.0f);

        left_shoulder->setPosition(glm::vec3(0.3, 0.2, 0.0));
        right_shoulder->setPosition(glm::vec3(-0.3, 0.2, 0.0));

        left_elbow->setPosition(glm::vec3(0.4, -0.5, 0.1 * r1));
        right_elbow->setPosition(glm::vec3(-0.4, -0.5, 0.1 * r3));

        left_hand->setPosition(glm::vec3(0.4, -1.5, 0.4 * r2));
        right_hand->setPosition(glm::vec3(-0.4, -1.5, 0.4 * r4));

        left_hip->setPosition(glm::vec3(0.2, -1.0, 0.0));
        right_hip->setPosition(glm::vec3(-0.2, -1.0, 0.0));

        left_knee->setPosition(glm::vec3(0.2, -1.8, 0.3 + 0.5 * s1));
        right_knee->setPosition(glm::vec3(-0.2, -1.8, 0.3 + 0.5 * s3));

        left_foot->setPosition(glm::vec3(0.2, -2.8, 0.3 + 0.8 * s2));
        right_foot->setPosition(glm::vec3(-0.2, -2.8, 0.3 + 0.8 * s4));
    }
    if (state == SIMPLEWALKER_STATE_RUNNING) {

        speed =2.0;
        physicalEntity->getRigidBody()->setLinearVelocity(bulletify3(1.6f * glm::normalize(target - walkerScene->transformation->getPosition())));

        float modulation = glm::fract(Application::instance->time);
        float modulation_halfforward = glm::fract(0.5 + Application::instance->time);
        float modulation_quadforward = glm::fract(0.75 + Application::instance->time);
        float avance = 0.95;
        float s1 = makemodulate2(speed, 0.0); // knee
        float s3 = makemodulate2(speed, 0.5); // knee
        float s2 = makemodulate2(speed, avance); // foot
        float s4 = makemodulate2(speed, 0.5f + avance - 1.0f); // foot

        float avance2 = 0.75;
        float r1 = makemodulate2(speed, 0.0);
        float r3 = makemodulate2(speed, 0.5);
        float r2 = makemodulate2(speed, avance2);
        float r4 = makemodulate2(speed, 0.5f + avance2 - 1.0f);

        left_shoulder->setPosition(glm::vec3(0.3, 0.2, 0.0));
        right_shoulder->setPosition(glm::vec3(-0.3, 0.2, 0.0));

        left_elbow->setPosition(glm::vec3(0.4, -0.5, 0.3 * r1));
        right_elbow->setPosition(glm::vec3(-0.4, -0.5, 0.3 * r3));

        left_hand->setPosition(glm::vec3(0.4, -1.5, 0.9 * r2));
        right_hand->setPosition(glm::vec3(-0.4, -1.5, 0.9 * r4));

        left_hip->setPosition(glm::vec3(0.2, -1.0, 0.0));
        right_hip->setPosition(glm::vec3(-0.2, -1.0, 0.0));

        left_knee->setPosition(glm::vec3(0.2, -1.8, 0.1 + 1.2 * s1));
        right_knee->setPosition(glm::vec3(-0.2, -1.8, 0.1 + 1.2 * s3));

        left_foot->setPosition(glm::vec3(0.2, -2.8 + abs(s2) * abs(s2) * 0.3, -0.2 + 1.7 * s2));
        right_foot->setPosition(glm::vec3(-0.2, -2.8 + abs(s4) * abs(s4) * 0.3, -0.2 + 1.7 * s4));
    }*/

   // glm::vec3 pos = glmify3(physicalEntity->getRigidBody()->getCenterOfMassPosition());
   // walkerScene->transformation->setPosition(pos);
   // auto lookat = glm::lookAt(glm::vec3(pos.x, target.y, pos.z), target, glm::vec3(0.0, 1.0, 0.0));
   // walkerScene->transformation->setOrientation(glm::quat(lookat));
    mesh_left_arm_up->getInstance(0)->transformation->setSize(glm::vec3(0.2f, 0.2f, 0.2f));
    mesh_left_arm_down->getInstance(0)->transformation->setSize(glm::vec3(0.2f, 0.2f, 0.2f));
    mesh_left_leg_up->getInstance(0)->transformation->setSize(glm::vec3(-0.1f, 0.1f, 0.1f));
    mesh_left_leg_down->getInstance(0)->transformation->setSize(glm::vec3(-0.1f, 0.1f, 0.1f));

    mesh_right_arm_up->getInstance(0)->transformation->setSize(glm::vec3(-0.2f, 0.2f, 0.2f));
    mesh_right_arm_down->getInstance(0)->transformation->setSize(glm::vec3(-0.2f, 0.2f, 0.2f));
    mesh_right_leg_up->getInstance(0)->transformation->setSize(glm::vec3(0.1f, 0.1f, 0.1f));
    mesh_right_leg_down->getInstance(0)->transformation->setSize(glm::vec3(0.1f, 0.1f, 0.1f));
    mesh_body->getInstance(0)->transformation->setSize(glm::vec3(0.1f));
    mesh_head->getInstance(0)->transformation->setSize(glm::vec3(0.1f));
return; //todo
}

void SimpleWalker::run(glm::vec3 position)
{
    state = SIMPLEWALKER_STATE_RUNNING;
    target = position;
}

void SimpleWalker::walk(glm::vec3 position)
{
    state = SIMPLEWALKER_STATE_WALKING;
    target = position;
}

void SimpleWalker::stop()
{
    state = SIMPLEWALKER_STATE_IDLE;
    target = mesh_body->getInstance(0)->transformation->getPosition();
}

void SimpleWalker::die()
{
}
