#pragma once
#include "Camera.h"
#include "Scene.h"
#include "Physics.h"
class World
{
public:
    World();
    ~World();
    Camera *mainDisplayCamera;
    Scene *scene;
    Physics *physics;
    void draw(VulkanRenderStage *stage, Camera *camera);
    void setUniforms(Camera *camera);
    void setSceneUniforms();
};
