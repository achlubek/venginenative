#pragma once 
#include "Mesh3d.h"
class Scene
{
public:
    Scene();
    ~Scene();
    /*
    for now steps are:
    update meshes buffers with frustum culling
    draw meshes with a shader (this time material)
    update lights (this inside uses draw meshes with shader (depth only))
    future:
    draw particles
    draw debug lines
    draw debug points
    */
    void draw(VulkanCommandBuffer cb);
    void prepareFrame();
    void addMesh3d(Mesh3d *item);
    vector<Mesh3d*>& getMesh3ds();
private:
    vector<Mesh3d*> mesh3ds;
};
