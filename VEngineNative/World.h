#pragma once
#include "Camera.h"
#include "Scene.h"
#include "Physics.h"
#include "ShaderProgram.h"
class World
{
public:
    World();
    ~World();
    Camera *mainDisplayCamera;
    Scene *scene;
    Physics *physics;
    void draw(ShaderProgram *shader, Camera *camera);
    void setUniforms(ShaderProgram *shader, Camera *camera);
    void setSceneUniforms();
};
