#include "stdafx.h"
#include "Scene.h"
#include "Game.h"

Scene::Scene()
{
    mesh3ds = {};
}

Scene::~Scene()
{
}

void Scene::draw(VulkanRenderStage* stage)
{
    int x = mesh3ds.size();
    for (int i = 0; i < x; i++) {
        mesh3ds[i]->draw(stage);
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


vector<Mesh3d*>& Scene::getMesh3ds()
{
    return mesh3ds;
}
