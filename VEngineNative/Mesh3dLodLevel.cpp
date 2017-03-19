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
    int kb1 = 1024;
    int mb1 = kb1 * 1024;
    modelInfosBuffer1 = new ShaderStorageBuffer();
    modelInfosBuffer2 = new ShaderStorageBuffer();
    modelInfosBuffer3 = new ShaderStorageBuffer();
    materialBuffer = new ShaderStorageBuffer(kb1);
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
	skeleton = nullptr;
	skeletonPose = nullptr;
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
    int kb1 = 1024;
    int mb1 = kb1 * 1024;
    modelInfosBuffer1 = new ShaderStorageBuffer();
    modelInfosBuffer2 = new ShaderStorageBuffer();
    modelInfosBuffer3 = new ShaderStorageBuffer();
    materialBuffer = new ShaderStorageBuffer(kb1);
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
	skeleton = nullptr;
	skeletonPose = nullptr;
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
    int kb1 = 1024;
    int mb1 = kb1 * 1024;
    modelInfosBuffer1 = new ShaderStorageBuffer();
    modelInfosBuffer2 = new ShaderStorageBuffer();
    modelInfosBuffer3 = new ShaderStorageBuffer();
    materialBuffer = new ShaderStorageBuffer(kb1);
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
	skeleton = nullptr;
	skeletonPose = nullptr;
    id = Game::instance->getNextId();
    Game::instance->registerId(id, this);
}

Mesh3dLodLevel::~Mesh3dLodLevel()
{
}
#define vcasti(a) (*((float*)&a))
void Mesh3dLodLevel::draw(const Mesh3d* mesh)
{
    ShaderProgram *shader = ShaderProgram::current;

    if (currentBuffer == 0 && instancesFiltered1 == 0) return;
    if (currentBuffer == 1 && instancesFiltered2 == 0) return;
    if (currentBuffer == 2 && instancesFiltered3 == 0) return;

    if (shader == Game::instance->shaders->idWriteShader && !mesh->selectable) return;
    if (shader == Game::instance->shaders->idWriteShader && !selectable) return;

    if (shader == Game::instance->shaders->depthOnlyShader && !mesh->castShadow) return;
    if (shader == Game::instance->shaders->depthOnlyShader && !castShadow) return;

    if (!mesh->visible) return;
    if (!visible) return;

    if (material->diffuseColorTex != nullptr) material->diffuseColorTex->use(10);
    if (material->normalTex != nullptr) material->normalTex->use(11);
    if (material->bumpTex != nullptr) material->bumpTex->use(12);
    if (material->roughnessTex != nullptr) material->roughnessTex->use(13);
    if (material->metalnessTex != nullptr) material->metalnessTex->use(14);

    // address 0  here
    /*
    shader->setUniform("Roughness", material->roughness);//1
    shader->setUniform("Metalness", material->metalness);//2
    shader->setUniform("DiffuseColor", material->diffuseColor);//2 + 4 rgba = 6

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
    shader->setUniform("LodLevelID", id);*/

    if (currentBuffer == 0)modelInfosBuffer1->use(0);
    if (currentBuffer == 1)modelInfosBuffer2->use(0);
    if (currentBuffer == 2)modelInfosBuffer3->use(0);

    materialBuffer->use(1);
    if (disableFaceCulling) glDisable(GL_CULL_FACE);
    if (currentBuffer == 0)info3d->drawInstanced(instancesFiltered1);
    if (currentBuffer == 1)info3d->drawInstanced(instancesFiltered2);
    if (currentBuffer == 2)info3d->drawInstanced(instancesFiltered3);
    if (disableFaceCulling) glEnable(GL_CULL_FACE);

}

void Mesh3dLodLevel::updateBuffer(const Mesh3d* mesh, const vector<Mesh3dInstance*> &instances)
{
    // next frame it runs current buffer to 1, next buffer will be 

    // buffers into nextbuffer, draws current
    vec3 cameraPos = Game::instance->world->mainDisplayCamera->transformation->getPosition();
    vector<Mesh3dInstance*> filtered;
    vector<int> newids;
    bool changed = false;
    for (int i = 0; i < instances.size(); i++) {
        float dst = distance(cameraPos, instances[i]->transformation->getPosition());
        if (instances[i]->visible && dst >= distanceStart && dst < distanceEnd && checkIntersection(instances[i])) {
            filtered.push_back(instances[i]);
            newids.push_back(instances[i]->id);
            if (instances[i]->transformation->needsUpdate) changed = true;
        }
    }
    if (newids.size() != lastIdMap.size()) {
        changed = true;
    }
    else {
        for (int i = 0; i < newids.size(); i++) {
            if (newids[i] != lastIdMap[i]) {
                changed = true;
                break;
            }
        }
    }
    lastIdMap.clear();
    lastIdMap = newids;
    
    if (changed) pendingUpdates = 3;

    /*layout rotation f4 translation f3+1 scale f3+1 =>> 12 floats*/
    // Urgently transfrom it into permament mapped buffer 

    int fint = filtered.size();

    if (pendingUpdates-- > 0) {

        currentBuffer++;
        if (currentBuffer > 2) currentBuffer = 0;

        if (currentBuffer == 0) nextBuffer = 2;
        if (currentBuffer == 1) nextBuffer = 0;
        if (currentBuffer == 2) nextBuffer = 1;

        if (nextBuffer == 0)instancesFiltered1 = fint;
        if (nextBuffer == 1)instancesFiltered2 = fint;
        if (nextBuffer == 2)instancesFiltered3 = fint;

        if (fint > 0) {
            void* modelbufferpt = nullptr;
            if (nextBuffer == 0)
                modelbufferpt = modelInfosBuffer1->acquireAsynchronousPointer(0, 4 * 16 * fint);
            if (nextBuffer == 1)
                modelbufferpt = modelInfosBuffer2->acquireAsynchronousPointer(0, 4 * 16 * fint);
            if (nextBuffer == 2)
                modelbufferpt = modelInfosBuffer3->acquireAsynchronousPointer(0, 4 * 16 * fint);
            int a = 0;
            for (unsigned int i = 0; i < fint; i++) {
                TransformationManager *mgr = filtered[i]->transformation;
                quat q = glm::inverse(mgr->getOrientation());
                vec3 p = mgr->getPosition();
                vec3 s = mgr->getSize();
                ((float*)modelbufferpt)[a++] = q.x;
                ((float*)modelbufferpt)[a++] = q.y;
                ((float*)modelbufferpt)[a++] = q.z;
                ((float*)modelbufferpt)[a++] = q.w;

                ((float*)modelbufferpt)[a++] = p.x;
                ((float*)modelbufferpt)[a++] = p.y;
                ((float*)modelbufferpt)[a++] = p.z;
                ((float*)modelbufferpt)[a++] = 1.0f;

                ((float*)modelbufferpt)[a++] = s.x;
                ((float*)modelbufferpt)[a++] = s.y;
                ((float*)modelbufferpt)[a++] = s.z;
                ((float*)modelbufferpt)[a++] = 1.0f;

                ((int*)modelbufferpt)[a++] = filtered[i]->id;
                ((int*)modelbufferpt)[a++] = filtered[i]->id;
                ((int*)modelbufferpt)[a++] = filtered[i]->id;
                ((int*)modelbufferpt)[a++] = filtered[i]->id;
            }

            if (nextBuffer == 0)
                modelInfosBuffer1->unmapBuffer();
            if (nextBuffer == 1)
                modelInfosBuffer2->unmapBuffer();
            if (nextBuffer == 2)
                modelInfosBuffer3->unmapBuffer();
        }
    }

    if (materialBufferNeedsUpdate) {
        materialBufferNeedsUpdate = false;
        vector<float> floats2;
        floats2.reserve(32);
        floats2.push_back(material->roughness);
        floats2.push_back(material->metalness);
        floats2.push_back(0.0f);
        floats2.push_back(0.0f);

        floats2.push_back(material->diffuseColor.x);
        floats2.push_back(material->diffuseColor.y);
        floats2.push_back(material->diffuseColor.z);
        floats2.push_back(0.0f);

        int one = 1;
        int zero = 0;

        floats2.push_back(vcasti((material->diffuseColorTex != nullptr ? one : zero)));
        floats2.push_back(vcasti((material->normalTex != nullptr ? one : zero)));
        floats2.push_back(vcasti((material->bumpTex != nullptr ? one : zero)));
        floats2.push_back(vcasti((material->roughnessTex != nullptr ? one : zero)));

        floats2.push_back(vcasti((material->metalnessTex != nullptr ? one : zero)));
        floats2.push_back(vcasti((skeleton != nullptr ? one : zero)));
        floats2.push_back(0.0f);
        floats2.push_back(0.0f); // amd safety 

        floats2.push_back(material->diffuseColorTexScale.x);
        floats2.push_back(material->diffuseColorTexScale.y);
        floats2.push_back(material->normalTexScale.x);
        floats2.push_back(material->normalTexScale.y);

        floats2.push_back(material->bumpTexScale.x);
        floats2.push_back(material->bumpTexScale.y);
        floats2.push_back(material->roughnessTexScale.x);
        floats2.push_back(material->roughnessTexScale.y);

        floats2.push_back(material->metalnessTexScale.x);
        floats2.push_back(material->metalnessTexScale.y);
        floats2.push_back(0.0f);
        floats2.push_back(0.0f);

        floats2.push_back(vcasti(mesh->id));
        floats2.push_back(vcasti(id));
        floats2.push_back(0.0f);
        floats2.push_back(0.0f);

        materialBuffer->mapData(4 * floats2.size(), floats2.data());
    }
}

float rsi2(vec3 ro, vec3 rd, vec3 sp, float sr)
{
    vec3 oc = ro - sp;
    float b = 2.0 * dot(rd, oc);
    float c = dot(oc, oc) - sr*sr;
    float disc = sqrt(b * b - 4.0 * c);
    vec2 ex = vec2(-b - disc, -b + disc) / 2.0f;
    return ex.x > 0.0 ? ex.x : ex.y;
}

vec4 projectvdao2(mat4 mat, vec3 pos) {
    return (mat * vec4(pos, 1.0));
}

bool Mesh3dLodLevel::checkIntersection(Mesh3dInstance * instance)
{
    auto size = instance->transformation->getSize();
    float radius = info3d->radius * glm::max(glm::max(size.x, size.y), size.z);

    vec3 center = instance->transformation->getPosition();// +0.5f * (info3d->aabbmax + info3d->aabbmin);
    vec3 camerapos = Game::instance->world->mainDisplayCamera->transformation->getPosition();

    float dst = distance(camerapos, center);

    if (dst < radius) {
        //  printf("SKIP");
        return true;
    }

    // i cannot find it on net so i craft my own cone/sphere test

    vec3 rdirC = Game::instance->world->mainDisplayCamera->cone->reconstructDirection(glm::vec2(0.5));
    vec3 helper = camerapos + rdirC * dst;
    vec3 helpdir = normalize(helper - center);
    vec3 newpos = center + helpdir * radius;

    vec4 prraw = projectvdao2(Game::instance->viewProjMatrix, newpos);
    vec2 pr = (vec2(prraw.x, prraw.y) / prraw.w) * 0.5f + 0.5f;
    bool sphereintersect = true;
    if (pr.x < 0.0 || pr.x > 1.0 || pr.y < 0.0 || pr.y > 1.0) sphereintersect = false;
    if (prraw.z < 0.0) sphereintersect = false;
    return sphereintersect;// || decidingdot > maxdt;
}
