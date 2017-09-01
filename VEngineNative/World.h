#pragma once
#include "Camera.h"
#include "Scene.h"
class World
{
public:
    World();
    ~World();
    Camera *mainDisplayCamera;
    Scene *scene;
    void draw(VulkanRenderStage *stage, Camera *camera);
    void setUniforms(Camera *camera);
    void setSceneUniforms();
};
