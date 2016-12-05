#include "stdafx.h"
#include "Mesh3d.h"

Mesh3d::Mesh3d()
{
    instances = {};
    lodLevels = {};
    needBufferUpdate = true;
}

Mesh3d::~Mesh3d()
{
}

Mesh3d * Mesh3d::create(Object3dInfo * info, Material * material)
{
    Mesh3d *m = new Mesh3d();
    m->addInstance(new Mesh3dInstance(new TransformationManager()));
    m->addLodLevel(new Mesh3dLodLevel(info, material));
    return m;
}

void Mesh3d::addInstance(Mesh3dInstance * instance)
{
    instances.push_back(instance);
    needBufferUpdate = true;
}

void Mesh3d::addLodLevel(Mesh3dLodLevel * level)
{
    lodLevels.push_back(level);
    needBufferUpdate = true;
}

void Mesh3d::clearInstances()
{
    instances.clear();
    needBufferUpdate = true;
}

void Mesh3d::clearLodLevels()
{
    lodLevels.clear();
    needBufferUpdate = true;
}

vector<Mesh3dInstance*>& Mesh3d::getInstances()
{
    return instances;
}

vector<Mesh3dLodLevel*>& Mesh3d::getLodLevels()
{
    return lodLevels;
}

Mesh3dInstance * Mesh3d::getInstance(int index)
{
    return instances[index];
}

Mesh3dLodLevel * Mesh3d::getLodLevel(int index)
{
    return lodLevels[index];
}

void Mesh3d::removeInstance(Mesh3dInstance* instance)
{
    for (int i = 0; i < instances.size(); i++) {
        if (instances[i] == instance) {
            instances.erase(instances.begin() + i);
            break;
        }
    }
    needBufferUpdate = true;
}

void Mesh3d::removeLodLevel(Mesh3dLodLevel* level)
{
    for (int i = 0; i < lodLevels.size(); i++) {
        if (lodLevels[i] == level) {
            lodLevels.erase(lodLevels.begin() + i);
            break;
        }
    }
    needBufferUpdate = true;
}

void Mesh3d::updateBuffers()
{
    for (int i = 0; i < lodLevels.size(); i++) {
        lodLevels[i]->updateBuffer(instances);
    }
    needBufferUpdate = false;
}

void Mesh3d::draw()
{
    for (int i = 0; i < lodLevels.size(); i++) {
        lodLevels[i]->draw();
    }
}

void Mesh3d::setUniforms()
{
    if (needBufferUpdate) {
        updateBuffers();
    }
    for (int i = 0; i < lodLevels.size(); i++) {
        lodLevels[i]->setUniforms();
    }
}