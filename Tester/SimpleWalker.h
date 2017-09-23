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

class SimpleWalker
{
public:
    SimpleWalker(Physics* physics, TransformationManager * spawn);
    ~SimpleWalker();
    char state = SIMPLEWALKER_STATE_WALKING;
      
    Scene* walkerScene;

    Physics* physics;
    PhysicalBody* physicalEntity;

    glm::vec3 target = glm::vec3(0.0);
    
    void update();

    void run(glm::vec3 position);
    void walk(glm::vec3 position);
    void stop();
    void die();
    Mesh3d * mesh_body;
    Mesh3d * mesh_head;
    Mesh3d * mesh_left_arm_up;
    Mesh3d * mesh_left_arm_down;
    Mesh3d * mesh_right_arm_up;
    Mesh3d * mesh_right_arm_down;

    Mesh3d * mesh_left_leg_up;
    Mesh3d * mesh_left_leg_down;
    Mesh3d * mesh_right_leg_up;
    Mesh3d * mesh_right_leg_down;

private:


    Mesh3d * debug_marker;
};

