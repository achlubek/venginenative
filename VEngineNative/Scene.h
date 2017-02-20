#pragma once
#include "EnvProbe.h"
#include "Light.h"
#include "Mesh3d.h"
#include "AbsDrawable.h";
class Scene
{
public:
    Scene();
    ~Scene();
    void draw();
    void setUniforms();
    void addDrawable(AbsDrawable *item);
    void addLight(Light *light);
    vector<AbsDrawable*>& getDrawables();
    vector<Light*>& getLights();
private:
    vector<AbsDrawable*> drawables;
    vector<Light*> lights;
};
