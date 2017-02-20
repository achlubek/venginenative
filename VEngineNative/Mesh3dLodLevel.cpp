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
    instancesFiltered1 = 0;
    instancesFiltered2 = 0;
    instancesFiltered3 = 0;
    modelInfosBuffer1 = new ShaderStorageBuffer();
    modelInfosBuffer2 = new ShaderStorageBuffer();
    modelInfosBuffer3 = new ShaderStorageBuffer();
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
    id = Game::instance->getNextId();
    Game::instance->registerId(id, this);
}

Mesh3dLodLevel::Mesh3dLodLevel(Object3dInfo *info, Material *imaterial)
{
    info3d = info;
    material = imaterial;
    distanceStart = 0;
    distanceEnd = 99999.0;
    instancesFiltered1 = 0;
    instancesFiltered2 = 0;
    instancesFiltered3 = 0;
    modelInfosBuffer1 = new ShaderStorageBuffer();
    modelInfosBuffer2 = new ShaderStorageBuffer();
    modelInfosBuffer3 = new ShaderStorageBuffer();
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
    id = Game::instance->getNextId();
    Game::instance->registerId(id, this);
}

Mesh3dLodLevel::Mesh3dLodLevel()
{
    info3d = nullptr;
    material = nullptr;
    distanceStart = 0;
    distanceEnd = 99999.0;
    instancesFiltered1 = 0;
    instancesFiltered2 = 0;
    instancesFiltered3 = 0;
    modelInfosBuffer1 = new ShaderStorageBuffer();
    modelInfosBuffer2 = new ShaderStorageBuffer();
    modelInfosBuffer3 = new ShaderStorageBuffer();
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
    id = Game::instance->getNextId();
    Game::instance->registerId(id, this);
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

    shader->setUniform("MeshID", mesh->id);
    shader->setUniform("LodLevelID", id);

    if (currentBuffer == 0)modelInfosBuffer1->use(0);
    if (currentBuffer == 1)modelInfosBuffer2->use(0);
    if (currentBuffer == 2)modelInfosBuffer3->use(0);

    if (currentBuffer == 0)info3d->drawInstanced(instancesFiltered1);
    if (currentBuffer == 1)info3d->drawInstanced(instancesFiltered2);
    if (currentBuffer == 2)info3d->drawInstanced(instancesFiltered3);

}

void Mesh3dLodLevel::updateBuffer(const vector<Mesh3dInstance*> &instances)
{
    vec3 cameraPos = Game::instance->world->mainDisplayCamera->transformation->getPosition();
    vector<Mesh3dInstance*> filtered;
    for (int i = 0; i < instances.size(); i++) {
        float dst = distance(cameraPos, instances[i]->transformation->getPosition());
        if (dst >= distanceStart && dst < distanceEnd && checkIntersection(instances[i])) {
            filtered.push_back(instances[i]);
        }
    }
    
    if (nextBuffer == 0)instancesFiltered1 = filtered.size();
    if (nextBuffer == 1)instancesFiltered2 = filtered.size();
    if (nextBuffer == 2)instancesFiltered3 = filtered.size();

    /*layout rotation f4 translation f3+1 scale f3+1 =>> 12 floats*/
    // Urgently transfrom it into permament mapped buffer
    vector<float> floats;
    int fint = 0;
    if (nextBuffer == 0)fint = instancesFiltered1;
    if (nextBuffer == 1)fint = instancesFiltered2;
    if (nextBuffer == 2)fint = instancesFiltered3;
    floats.reserve(16 * fint);
    for (unsigned int i = 0; i < fint; i++) {
        TransformationManager *mgr = filtered[i]->transformation;
        quat q = mgr->getOrientation();
        vec3 p = mgr->getPosition();
        vec3 s = mgr->getSize();
        floats.push_back(q.x);
        floats.push_back(q.y);
        floats.push_back(q.z);
        floats.push_back(q.w);

        floats.push_back(p.x);
        floats.push_back(p.y);
        floats.push_back(p.z);
        floats.push_back(1);

        floats.push_back(s.x);
        floats.push_back(s.y);
        floats.push_back(s.z);
        floats.push_back(1);

        unsigned int * tmp1 = &filtered[i]->id;
        float* tmpf = (float*)tmp1;
        float specialid = *tmpf;
        floats.push_back(specialid);
        floats.push_back(specialid);
        floats.push_back(specialid);
        floats.push_back(specialid);
    }
    if (nextBuffer == 0)
        modelInfosBuffer1->mapData(4 * floats.size(), floats.data());
    if (nextBuffer == 1)
        modelInfosBuffer2->mapData(4 * floats.size(), floats.data());
    if (nextBuffer == 2)
        modelInfosBuffer3->mapData(4 * floats.size(), floats.data());
    currentBuffer++;
    nextBuffer++;
    if (currentBuffer > 2) currentBuffer = 0;
    if (nextBuffer > 2) nextBuffer = 0;
}

bool Mesh3dLodLevel::checkIntersection(Mesh3dInstance * instance)
{
    float radius = glm::max(info3d->aabbmax.length(), info3d->aabbmin.length());

    vec3 center = instance->transformation->getPosition() + 0.5f * (info3d->aabbmax + info3d->aabbmin);

    float dst = distance(Game::instance->world->mainDisplayCamera->transformation->getPosition(), center);
    if (radius * 4.0f > dst) {
        return true;
    }

    glm::vec3 ro = Game::instance->world->mainDisplayCamera->transformation->getPosition();

    // i cannot find it on net so i craft my own cone/sphere test

    vec3 rdirC = Game::instance->world->mainDisplayCamera->cone->reconstructDirection(glm::vec2(0.5));
    vec3 rdir0 = Game::instance->world->mainDisplayCamera->cone->reconstructDirection(glm::vec2(0.0));
    float maxdt = dot(rdirC, rdir0);
    vec3 helper = ro + rdirC * dst;
    vec3 helpdir = normalize(helper - center);
    vec3 newpos = center + helpdir * radius;
    vec3 newdir = normalize(newpos - ro);
    float decidingdot = dot(newdir, rdirC);
    return decidingdot > maxdt;
}
