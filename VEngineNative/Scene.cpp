#include "stdafx.h"
#include "Scene.h"
#include "Game.h"

Scene::Scene()
{
    drawables = {};
    lights = {};
}

Scene::~Scene()
{
}

void Scene::draw()
{
    int x = drawables.size();
    for (int i = 0; i < x; i++) {
        drawables[i]->draw();
    }
}

void Scene::setUniforms()
{
    int x = drawables.size();
    for (int i = 0; i < x; i++) {
        drawables[i]->setUniforms();
    }
}

void Scene::addDrawable(AbsDrawable * item)
{
    drawables.push_back(item);
}

void Scene::addLight(Light * light)
{
    lights.push_back(light);
}


vector<AbsDrawable*>& Scene::getDrawables()
{
    return drawables;
}

vector<Light*>& Scene::getLights()
{
    return lights;
}
