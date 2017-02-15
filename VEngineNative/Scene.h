#pragma once
#include "EnvProbe.h"
#include "Light.h"
#include "Mesh3d.h"
#include "AbsDrawable.h";
class Scene : public AbsDrawable
{
public:
    Scene();
    ~Scene();
    void draw();
    void setUniforms(TransformStruct transform);
    void addDrawable(AbsDrawable *item);
    void addLight(Light *light);
    vector<AbsDrawable*>& getDrawables();
    vector<Light*>& getLights();
private:
    vector<AbsDrawable*> drawables;
    vector<Light*> lights;
};
