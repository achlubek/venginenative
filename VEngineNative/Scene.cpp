#include "stdafx.h"
#include "Scene.h"
#include "Game.h"

Scene::Scene()
{
    mesh3ds = {};
    lights = {};
}

Scene::~Scene()
{
}

void Scene::draw()
{
    int x = mesh3ds.size();
    for (int i = 0; i < x; i++) {
        mesh3ds[i]->draw();
    }
}

void Scene::prepareFrame()
{
    int x = mesh3ds.size();
    for (int i = 0; i < x; i++) {
        mesh3ds[i]->setUniforms();
    }
}

void Scene::addMesh3d(Mesh3d * item)
{
    mesh3ds.push_back(item);
}

void Scene::addLight(Light * light)
{
    lights.push_back(light);
}


vector<Mesh3d*>& Scene::getMesh3ds()
{
    return mesh3ds;
}

vector<Light*>& Scene::getLights()
{
    return lights;
}
