#include "stdafx.h"
#include "Scene.h" 
#include "Mesh3d.h" 

Scene::Scene()
{
    initTransformation();
    mesh3ds = {};
    scenes = {};
}

Scene::Scene(TransformationManager *transmgr)
{
    transformation = transmgr;
    mesh3ds = {};
    scenes = {};
}

Scene::~Scene()
{
}

void Scene::draw(glm::mat4 parentTransform, VulkanRenderStage* stage)
{
    auto trans = transformation->getWorldTransform() * parentTransform;
    int x = mesh3ds.size();
    for (int i = 0; i < x; i++) {
        mesh3ds[i]->draw(stage);
    }

    x = scenes.size();
    for (int i = 0; i < x; i++) {
        scenes[i]->draw(trans, stage);
    }
}

void Scene::prepareFrame(glm::mat4 parentTransform)
{
    auto trans = transformation->getWorldTransform() * parentTransform;
    int x = mesh3ds.size();
    for (int i = 0; i < x; i++) {
        mesh3ds[i]->setUniforms(trans);
    }
    
    x = scenes.size();
    for (int i = 0; i < x; i++) {
        scenes[i]->prepareFrame(trans);
    }
}

void Scene::addMesh3d(Mesh3d * item)
{
    mesh3ds.push_back(item);
}

void Scene::addScene(Scene * item)
{
    scenes.push_back(item);
}

vector<Mesh3d*>& Scene::getMesh3ds()
{
    return mesh3ds;
}

vector<Scene*>& Scene::getScenes()
{
    return scenes;
}
