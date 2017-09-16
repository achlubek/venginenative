#pragma once
class Physics;
class TransformationManager;
class PhysicalBody;
class PhysicalConstraint;
class INIReader;
class Mesh3d;
class TransformStruct;

#define SIMPLEWALKER_STATE_IDLE 1
#define SIMPLEWALKER_STATE_WALKING 2
#define SIMPLEWALKER_STATE_RUNNING 3
#define SIMPLEWALKER_STATE_DEAD 4

struct Simple2PointAlignment {
public:
    TransformationManager * p1,  *p2;
    Simple2PointAlignment() {}
    Simple2PointAlignment(TransformationManager * ip1, TransformationManager * ip2):
    p1(ip1), p2(ip2) {

    }

    glm::quat getOrientation() {
        auto a = glm::lookAt(glm::vec3(0.0), glm::normalize(p2->getPosition() - p1->getPosition()), glm::vec3(0.0, 0.001, 1.0));
        return glm::quat(a);
    }

    glm::vec3 getCenter() {
        return (p1->getPosition() + p2->getPosition()) * 0.5f;
    }

};

class SimpleWalker
{
public:
    SimpleWalker(Physics* physics, TransformationManager * spawn);
    ~SimpleWalker();
    char state = SIMPLEWALKER_STATE_WALKING;

    TransformationManager * left_shoulder;
    TransformationManager * left_elbow;
    TransformationManager * left_hand;
    TransformationManager * right_shoulder;
    TransformationManager * right_elbow;
    TransformationManager * right_hand;
    TransformationManager * left_hip;
    TransformationManager * left_knee;
    TransformationManager * left_foot;
    TransformationManager * right_hip;
    TransformationManager * right_knee;
    TransformationManager * right_foot;

    Simple2PointAlignment bone_left_arm_up;
    Simple2PointAlignment bone_left_arm_down;
    Simple2PointAlignment bone_right_arm_up;
    Simple2PointAlignment bone_right_arm_down;

    Simple2PointAlignment bone_left_leg_up;
    Simple2PointAlignment bone_left_leg_down;
    Simple2PointAlignment bone_right_leg_up;
    Simple2PointAlignment bone_right_leg_down;



    Scene* walkerScene;

    Physics* physics;
    PhysicalBody* physicalEntity;

    glm::vec3 target = glm::vec3(0.0);

    void update();

    void run(glm::vec3 position);
    void walk(glm::vec3 position);
    void stop();
    void die();
private:

    Mesh3d * mesh_left_arm_up;
    Mesh3d * mesh_left_arm_down;
    Mesh3d * mesh_right_arm_up;
    Mesh3d * mesh_right_arm_down;

    Mesh3d * mesh_left_leg_up;
    Mesh3d * mesh_left_leg_down;
    Mesh3d * mesh_right_leg_up;
    Mesh3d * mesh_right_leg_down;

    Mesh3d * mesh_body;

    Mesh3d * debug_marker;
};

