#include "stdafx.h"
#include "SimpleWalker.h"
#include "Mesh3d.h"



SimpleWalker::SimpleWalker(Physics * physics, TransformationManager * spawn)
{
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

    bone_left_arm_up = Simple2PointAlignment(left_elbow, left_shoulder);
    bone_left_arm_down = Simple2PointAlignment(left_hand, left_elbow);
    bone_right_arm_up = Simple2PointAlignment(right_elbow, right_shoulder);
    bone_right_arm_down = Simple2PointAlignment(right_hand, right_elbow);

    bone_left_leg_up = Simple2PointAlignment(left_knee, left_hip);
    bone_left_leg_down = Simple2PointAlignment(left_foot, left_knee);
    bone_right_leg_up = Simple2PointAlignment(right_knee, right_hip);
    bone_right_leg_down = Simple2PointAlignment(right_foot, right_knee);

    auto material = new Material();
    mesh_left_arm_up = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_arm_up_left.raw"), material);
    mesh_left_arm_down = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_arm_down_left.raw"), material);
    mesh_right_arm_up = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_arm_up_right.raw"), material);
    mesh_right_arm_down = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_arm_down_right.raw"), material);

    mesh_left_leg_up = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_leg_up_left.raw"), material);
    mesh_left_leg_down = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_leg_down_left.raw"), material);
    mesh_right_leg_up = Mesh3d::create(Application::instance->asset->loadObject3dInfoFile("skeleton_leg_up_right.raw"), material);
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

void SimpleWalker::update()
{
    if (state == SIMPLEWALKER_STATE_IDLE) {

        float modulation = glm::fract(Application::instance->time);
        float modulation_halfforward = glm::fract(0.5 + Application::instance->time);
        float modulation_quadforward = glm::fract(0.75 + Application::instance->time);
        float speed = 0.3;
        float s1 = makemodulate(speed, 0.0, 0.3);
        float s2 = makemodulate(speed, 0.75, 0.2);
        float s3 = makemodulate(speed, 0.5, 0.3);
        float s4 = makemodulate(speed, 0.25, 0.2);

        float r1 = makemodulate(speed, 0.0, 0.7);
        float r2 = makemodulate(speed, 0.75, 0.7);
        float r3 = makemodulate(speed, 0.5, 0.7);
        float r4 = makemodulate(speed, 0.25, 0.7);

        left_shoulder->setPosition(glm::vec3(0.1, 0.4, 0.0));
        right_shoulder->setPosition(glm::vec3(-0.1, 0.4, 0.0));

        left_elbow->setPosition(glm::vec3(0.4, -0.1, 0.1 * r3));
        right_elbow->setPosition(glm::vec3(-0.4, -0.1, 0.1 * r1));

        left_hand->setPosition(glm::vec3(0.4, -1.6,  0.1 * r4));
        right_hand->setPosition(glm::vec3(-0.4, -1.6,  0.1 * r2));

        left_hip->setPosition(glm::vec3(0.2, -1.0, 0.0));
        right_hip->setPosition(glm::vec3(-0.2, -1.0, 0.0));

        left_knee->setPosition(glm::vec3(0.2, -2.0, 0.3 + 0.8 * s1));
        right_knee->setPosition(glm::vec3(-0.2, -2.0, 0.3 + 0.8 * s3));

        left_foot->setPosition(glm::vec3(0.2, -3.6, 0.3 + 0.8 * s2));
        right_foot->setPosition(glm::vec3(-0.2, -3.6, 0.3 + 0.8 * s4));
    }

    auto pos = mesh_body->getInstance(0)->transformation->getPosition(); 
    glm::mat4 a = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    auto rotatefix = glm::quat(glm::rotate(a, PI / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f)));

    auto Zrotfix1L = glm::quat(glm::rotate(a, 0.4f, glm::vec3(0.0f, 0.0f, 1.0f)));
    auto Zrotfix1R = glm::quat(glm::rotate(a, -0.4f, glm::vec3(0.0f, 0.0f, 1.0f)));

    auto Xrotfix1 = glm::quat(glm::rotate(a, -0.3f, glm::vec3(1.0f, 0.0f, 0.0f)));

    mesh_left_arm_up->getInstance(0)->transformation->setOrientation(Zrotfix1L * rotatefix * bone_left_arm_up.getOrientation());
    mesh_left_arm_up->getInstance(0)->transformation->setPosition(pos + bone_left_arm_up.getCenter());
    mesh_left_arm_up->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_left_arm_down->getInstance(0)->transformation->setOrientation(Xrotfix1 * Zrotfix1L * rotatefix * bone_left_arm_down.getOrientation());
    mesh_left_arm_down->getInstance(0)->transformation->setPosition(pos + bone_left_arm_down.getCenter());
    mesh_left_arm_down->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_right_arm_up->getInstance(0)->transformation->setOrientation(Zrotfix1R * rotatefix * bone_right_arm_up.getOrientation());
    mesh_right_arm_up->getInstance(0)->transformation->setPosition(pos + bone_right_arm_up.getCenter());
    mesh_right_arm_up->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_right_arm_down->getInstance(0)->transformation->setOrientation(Xrotfix1 * Zrotfix1R * rotatefix * bone_right_arm_down.getOrientation());
    mesh_right_arm_down->getInstance(0)->transformation->setPosition(pos + bone_right_arm_down.getCenter());
    mesh_right_arm_down->getInstance(0)->transformation->setSize(glm::vec3(0.1f));


    mesh_left_leg_up->getInstance(0)->transformation->setOrientation(rotatefix * bone_left_leg_up.getOrientation());
    mesh_left_leg_up->getInstance(0)->transformation->setPosition(pos + bone_left_leg_up.getCenter());
    mesh_left_leg_up->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_left_leg_down->getInstance(0)->transformation->setOrientation(rotatefix * bone_left_leg_down.getOrientation());
    mesh_left_leg_down->getInstance(0)->transformation->setPosition(pos + bone_left_leg_down.getCenter());
    mesh_left_leg_down->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_right_leg_up->getInstance(0)->transformation->setOrientation(rotatefix * bone_right_leg_up.getOrientation());
    mesh_right_leg_up->getInstance(0)->transformation->setPosition(pos + bone_right_leg_up.getCenter());
    mesh_right_leg_up->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_right_leg_down->getInstance(0)->transformation->setOrientation(rotatefix * bone_right_leg_down.getOrientation());
    mesh_right_leg_down->getInstance(0)->transformation->setPosition(pos + bone_right_leg_down.getCenter());
    mesh_right_leg_down->getInstance(0)->transformation->setSize(glm::vec3(0.1f));

    mesh_body->getInstance(0)->transformation->setSize(glm::vec3(0.1f));
}
