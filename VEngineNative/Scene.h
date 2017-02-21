#pragma once
#include "EnvProbe.h"
#include "Light.h"
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
    void draw();
    void prepareFrame();
    void addMesh3d(Mesh3d *item);
    void addLight(Light *light);
    vector<Mesh3d*>& getMesh3ds();
    vector<Light*>& getLights();
private:
    vector<Mesh3d*> mesh3ds;
    vector<Light*> lights;
};
