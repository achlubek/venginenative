#include "stdafx.h"
#include "Scene.h"
#include "Game.h"

Scene::Scene()
{
    meshes = {};
    lights = {};
}

Scene::~Scene()
{
}

void Scene::draw()
{
    int x = meshes.size();
    for (int i = 0; i < x; i++) {
        meshes[i]->draw();
    }
}

void Scene::setUniforms()
{
    int x = meshes.size();
    for (int i = 0; i < x; i++) {
        meshes[i]->setUniforms();
    }
}

void Scene::addMesh(Mesh3d * mesh)
{
    meshes.push_back(mesh);
}

void Scene::addLight(Light * light)
{
    lights.push_back(light);
}

void Scene::addEnvProbe(EnvProbe * env)
{
    envProbes.push_back(env);
}

vector<Mesh3d*>& Scene::getMeshes()
{
    return meshes;
}

vector<Light*>& Scene::getLights()
{
    return lights;
}

vector<EnvProbe*>& Scene::getEnvProbes()
{
    return envProbes;
}