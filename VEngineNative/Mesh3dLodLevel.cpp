#include "stdafx.h"
#include "Game.h"
#include "Mesh3dLodLevel.h"

using namespace glm;

Mesh3dLodLevel::Mesh3dLodLevel(Object3dInfo *info, Material *imaterial, float distancestart, float distanceend)
{
    info3d = info;
    material = imaterial;
    distanceStart = distancestart;
    distanceEnd = distanceend;
    instancesFiltered = 0;
    modelInfosBuffer = new ShaderStorageBuffer();
    samplerIndices = {};
    modes = {};
    targets = {};
    sources = {};
    modifiers = {};
    uvScales = {};
    nodesDatas = {};
    nodesColors = {};
    modes = {};
    textureBinds = {};
    wrapModes = {};
}

Mesh3dLodLevel::Mesh3dLodLevel(Object3dInfo *info, Material *imaterial)
{
    info3d = info;
    material = imaterial;
    distanceStart = 0;
    distanceEnd = 99999.0;
    instancesFiltered = 0;
    modelInfosBuffer = new ShaderStorageBuffer();
    samplerIndices = {};
    modes = {};
    targets = {};
    sources = {};
    modifiers = {};
    uvScales = {};
    nodesDatas = {};
    nodesColors = {};
    modes = {};
    textureBinds = {};
    wrapModes = {};
}

Mesh3dLodLevel::Mesh3dLodLevel()
{
    info3d = nullptr;
    material = nullptr;
    distanceStart = 0;
    distanceEnd = 99999.0;
    instancesFiltered = 0;
    modelInfosBuffer = new ShaderStorageBuffer();
    samplerIndices = {};
    modes = {};
    targets = {};
    sources = {};
    modifiers = {};
    uvScales = {};
    nodesDatas = {};
    nodesColors = {};
    modes = {};
    textureBinds = {};
    wrapModes = {};
}

Mesh3dLodLevel::~Mesh3dLodLevel()
{
}

void Mesh3dLodLevel::draw(const Mesh3d* mesh)
{

    ShaderProgram *shader = ShaderProgram::current;

    if (material->diffuseColorTex != nullptr) material->diffuseColorTex->use(10);
    if (material->normalTex != nullptr) material->normalTex->use(11);
    if (material->bumpTex != nullptr) material->bumpTex->use(12);
    if (material->roughnessTex != nullptr) material->roughnessTex->use(13);
    if (material->metalnessTex != nullptr) material->metalnessTex->use(14);

    shader->setUniform("Roughness", material->roughness);
    shader->setUniform("Metalness", material->metalness);
    shader->setUniform("DiffuseColor", material->diffuseColor);

    shader->setUniform("useDiffuseColorTexInt", material->diffuseColorTex != nullptr ? 1 : 0);
    shader->setUniform("useNormalTexInt", material->normalTex != nullptr ? 1 : 0);
    shader->setUniform("useBumpTexInt", material->bumpTex != nullptr ? 1 : 0);
    shader->setUniform("useRoughnessTexInt", material->roughnessTex != nullptr ? 1 : 0);
    shader->setUniform("useMetalnessTexInt", material->metalnessTex != nullptr ? 1 : 0);

    shader->setUniform("diffuseColorTexScale", material->diffuseColorTexScale);
    shader->setUniform("normalTexScale", material->normalTexScale);
    shader->setUniform("bumpTexScale", material->bumpTexScale);
    shader->setUniform("roughnessTexScale", material->roughnessTexScale);
    shader->setUniform("metalnessTexScale", material->metalnessTexScale);

    shader->setUniform("Id", mesh->Id);

    modelInfosBuffer->use(0);

    info3d->drawInstanced(instancesFiltered);
}

void Mesh3dLodLevel::updateBuffer(const vector<Mesh3dInstance*> &instances)
{
    vec3 cameraPos = Game::instance->world->mainDisplayCamera->transformation->position;
    vector<Mesh3dInstance*> filtered;
    for (int i = 0; i < instances.size(); i++) {
        float dst = distance(cameraPos, instances[i]->transformation->position);
        if (dst >= distanceStart && dst < distanceEnd && checkIntersection(instances[i])) {
            filtered.push_back(instances[i]);
        }
    }
    instancesFiltered = filtered.size();
    /*layout rotation f4 translation f3+1 scale f3+1 =>> 12 floats*/
    // Urgently transfrom it into permament mapped buffer
    vector<float> floats;
    floats.reserve(12 * instancesFiltered);
    for (unsigned int i = 0; i < instancesFiltered; i++) {
        TransformationManager *mgr = filtered[i]->transformation;
        floats.push_back(mgr->orientation.x);
        floats.push_back(mgr->orientation.y);
        floats.push_back(mgr->orientation.z);
        floats.push_back(mgr->orientation.w);

        floats.push_back(mgr->position.x);
        floats.push_back(mgr->position.y);
        floats.push_back(mgr->position.z);
        floats.push_back(1);

        floats.push_back(mgr->size.x);
        floats.push_back(mgr->size.y);
        floats.push_back(mgr->size.z);
        floats.push_back(1);
    }
    modelInfosBuffer->mapData(4 * floats.size(), floats.data());
}

bool Mesh3dLodLevel::checkIntersection(Mesh3dInstance * instance)
{
    float radius = glm::max(info3d->aabbmax.length(), info3d->aabbmin.length());

    float dst = distance(Game::instance->world->mainDisplayCamera->transformation->position, instance->transformation->position);
    if (radius * 2.0f > dst) {
        return true;
    }

    glm::vec3 ro = Game::instance->world->mainDisplayCamera->transformation->position;

    // i cannot find it on net so i craft my own cone/sphere test

    vec3 rdirC = Game::instance->world->mainDisplayCamera->cone->reconstructDirection(glm::vec2(0.5));
    vec3 rdir0 = Game::instance->world->mainDisplayCamera->cone->reconstructDirection(glm::vec2(0.0));
    float maxdt = dot(rdirC, rdir0);
    vec3 helper = ro + rdirC * dst;
    vec3 helpdir = normalize(helper - instance->transformation->position);
    vec3 newpos = instance->transformation->position + helpdir * radius;
    vec3 newdir = normalize(newpos - ro);
    float decidingdot = dot(newdir, rdirC);
    return decidingdot > maxdt;
}
