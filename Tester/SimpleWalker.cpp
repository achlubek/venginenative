#include "stdafx.h"
#include "SimpleWalker.h"
#include "Mesh3d.h"
#include "Physics.h"
#include "PhysicalBody.h"



SimpleWalker::SimpleWalker(Physics * iphysics, TransformationManager * spawn)
    : physics(iphysics)
{
    walkerScene = new Scene(spawn);
    left_shoulder = new TransformationManager();
    right_shoulder = new TransformationManager();

    left_elbow = new TransformationManager();
    right_elbow = new TransformationManager();

    left_hand = new TransformationManager();
    right_hand = new TransformationManager();

    left_hip = new TransformationManager();
    right_hip = new TransformationManager();

    left_knee = new TransformationManager();
    right_knee = new TransformationManager();

    left_foot = new TransformationManager();
    right_foot = new TransformationManager();

    head = new TransformationManager();
    ass = new TransformationManager();

    bone_body = Simple2PointAlignment(ass, head);

    bone_left_arm_up = Simple2PointAlignment(left_elbow, left_shoulder);
    bone_left_arm_down = Simple2PointAlignment(left_hand, left_elbow);
    bone_right_arm_up = Simple2PointAlignment(right_elbow, right_shoulder);
    bone_right_arm_down = Simple2PointAlignment(right_hand, right_elbow);

    bone_left_leg_up = Simple2PointAlignment(left_knee, left_hip);
    bone_left_leg_down = Simple2PointAlignment(left_foot, left_knee);
    bone_right_leg_up = Simple2PointAlignment(right_knee, right_hip);
    bone_right_leg_down = Simple2PointAlignment(right_foot, right_knee);

    auto material = new Material();
    mesh_left_arm_up    = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_arm_up_left.raw"), material);
    mesh_left_arm_down  = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_arm_down_left.raw"), material);
    mesh_right_arm_up   = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_arm_up_right.raw"), material);
    mesh_right_arm_down = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_arm_down_right.raw"), material);

    mesh_left_leg_up    = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_leg_up_left.raw"), material);
    mesh_left_leg_down  = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_leg_down_left.raw"), material);
    mesh_right_leg_up   = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_leg_up_right.raw"), material);
    mesh_right_leg_down = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_leg_down_right.raw"), material);


    mesh_body = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_body.raw"), material);
     

    debug_marker = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("icos.raw"), material);
    debug_marker->addInstance(new Mesh3dInstance(left_shoulder));
    debug_marker->addInstance(new Mesh3dInstance(right_shoulder));
    debug_marker->addInstance(new Mesh3dInstance(left_elbow));
    debug_marker->addInstance(new Mesh3dInstance(right_elbow));
    debug_marker->addInstance(new Mesh3dInstance(left_hand));
    debug_marker->addInstance(new Mesh3dInstance(right_hand));
    debug_marker->addInstance(new Mesh3dInstance(left_hip));
    debug_marker->addInstance(new Mesh3dInstance(right_hip));
    debug_marker->addInstance(new Mesh3dInstance(left_knee));
    debug_marker->addInstance(new Mesh3dInstance(right_knee));
    debug_marker->addInstance(new Mesh3dInstance(left_foot));
    debug_marker->addInstance(new Mesh3dInstance(right_foot));
    for (int i = 0; i < debug_marker->getInstances().size(); i++) {
        debug_marker->getInstance(i)->transformation->setSize(glm::vec3(0.05));
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
    walkerScene->addMesh3d(debug_marker);
    

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

    glm::vec3 pos = glmify3(physicalEntity->getRigidBody()->getCenterOfMassPosition());
    walkerScene->transformation->setPosition(pos);
   // auto lookat = glm::lookAt(glm::vec3(pos.x, target.y, pos.z), target, glm::vec3(0.0, 1.0, 0.0));
   // walkerScene->transformation->setOrientation(glm::quat(lookat));

    auto oriento = walkerScene->transformation->getOrientation();
    
    glm::mat4 a = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    auto rotatefix = glm::quat(glm::rotate(a, 0.0f * PI / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f)));

    auto legstranslatefix = glm::vec3(0.0, -0.45, 0.0);
    auto shoulderposfixL = glm::vec3(-0.1, 0.2, 0.0);
    auto shoulderposfixR = (shoulderposfixL * glm::vec3(-1.0, 1.0, 1.0));

    auto Zrotfix1L = glm::quat(glm::rotate(a,-1.1f, glm::vec3(0.0f, 0.0f, 1.0f)));
    auto Zrotfix1R = glm::quat(glm::rotate(a, 1.1f, glm::vec3(0.0f, 0.0f, 1.0f)));
    auto Z2rotfix1L = glm::quat(glm::rotate(a, -0.8f, glm::vec3(0.0f, 0.0f, 1.0f)));
    auto Z2rotfix1R = glm::quat(glm::rotate(a, 0.8f, glm::vec3(0.0f, 0.0f, 1.0f)));


    auto Z2rotfix2L = glm::quat(glm::rotate(a, -0.4f, glm::vec3(0.0f, 0.0f, 1.0f)));
    auto Z2rotfix2R = glm::quat(glm::rotate(a, 0.4f, glm::vec3(0.0f, 0.0f, 1.0f)));
    
    auto Z2rotfix3L = glm::quat(glm::rotate(a, -0.4f, glm::vec3(0.0f, 0.0f, 1.0f)));
    auto Z2rotfix3R = glm::quat(glm::rotate(a, 0.4f, glm::vec3(0.0f, 0.0f, 1.0f)));

    auto Xrotfix1 = glm::quat(glm::rotate(a, -0.3f, glm::vec3(1.0f, 0.0f, 0.0f)));

    mesh_left_arm_up->getInstance(0)->transformation->setOrientation(Zrotfix1L * rotatefix * bone_left_arm_up.getOrientation());
    mesh_left_arm_up->getInstance(0)->transformation->setPosition(shoulderposfixL + bone_left_arm_up.getCenter());
    mesh_left_arm_up->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_left_arm_down->getInstance(0)->transformation->setOrientation(Z2rotfix1L * rotatefix * bone_left_arm_down.getOrientation());
    mesh_left_arm_down->getInstance(0)->transformation->setPosition( bone_left_arm_down.getCenter());
    mesh_left_arm_down->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_right_arm_up->getInstance(0)->transformation->setOrientation(Zrotfix1R * rotatefix * bone_right_arm_up.getOrientation());
    mesh_right_arm_up->getInstance(0)->transformation->setPosition(shoulderposfixR + bone_right_arm_up.getCenter());
    mesh_right_arm_up->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_right_arm_down->getInstance(0)->transformation->setOrientation(Z2rotfix1R * rotatefix * bone_right_arm_down.getOrientation());
    mesh_right_arm_down->getInstance(0)->transformation->setPosition(bone_right_arm_down.getCenter());
    mesh_right_arm_down->getInstance(0)->transformation->setSize(glm::vec3(0.1f));


    mesh_left_leg_up->getInstance(0)->transformation->setOrientation(Z2rotfix2L * rotatefix * bone_left_leg_up.getOrientation());
    mesh_left_leg_up->getInstance(0)->transformation->setPosition(bone_left_leg_up.getCenter());
    mesh_left_leg_up->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_left_leg_down->getInstance(0)->transformation->setOrientation(Z2rotfix3L * rotatefix * bone_left_leg_down.getOrientation());
    mesh_left_leg_down->getInstance(0)->transformation->setPosition(legstranslatefix + bone_left_leg_down.getCenter());
    mesh_left_leg_down->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_right_leg_up->getInstance(0)->transformation->setOrientation(Z2rotfix2R * rotatefix * bone_right_leg_up.getOrientation());
    mesh_right_leg_up->getInstance(0)->transformation->setPosition(bone_right_leg_up.getCenter());
    mesh_right_leg_up->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_right_leg_down->getInstance(0)->transformation->setOrientation(Z2rotfix3R * rotatefix * bone_right_leg_down.getOrientation());
    mesh_right_leg_down->getInstance(0)->transformation->setPosition(legstranslatefix + bone_right_leg_down.getCenter());
    mesh_right_leg_down->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_body->getInstance(0)->transformation->setOrientation(rotatefix * bone_body.getOrientation());
    mesh_body->getInstance(0)->transformation->setPosition(bone_body.getCenter());
    mesh_body->getInstance(0)->transformation->setSize(glm::vec3(0.1f));
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
