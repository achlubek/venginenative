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
    drawInfoBuffer = new ShaderStorageBuffer();
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
    drawInfoBuffer = new ShaderStorageBuffer();
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
    drawInfoBuffer = new ShaderStorageBuffer();
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

void Mesh3dLodLevel::draw()
{
    if (useGeometryShader && ShaderProgram::current == Game::instance->shaders->depthOnlyShader) {
        Game::instance->shaders->depthOnlyGeometryShader->use();
    }
    if (!useGeometryShader && ShaderProgram::current == Game::instance->shaders->depthOnlyGeometryShader) {
        Game::instance->shaders->depthOnlyShader->use();
    }
    if (useGeometryShader && ShaderProgram::current == Game::instance->shaders->materialShader) {
        Game::instance->shaders->materialGeometryShader->use();
    }
    if (!useGeometryShader && ShaderProgram::current == Game::instance->shaders->materialGeometryShader) {
        Game::instance->shaders->materialShader->use();
    }

    ShaderProgram *shader = ShaderProgram::current;
    shader->setUniform("Roughness", material->roughness);
    shader->setUniform("Metalness", material->metalness);
    shader->setUniform("DiffuseColor", material->diffuseColor);
    shader->setUniform("NodesCount", (int)material->nodes.size());
    shader->setUniformVector("SamplerIndexArray", samplerIndices);
    shader->setUniformVector("ModeArray", modes);
    shader->setUniformVector("TargetArray", targets);
    shader->setUniformVector("SourcesArray", sources);
    shader->setUniformVector("ModifiersArray", modifiers);
    shader->setUniformVector("UVScaleArray", uvScales);
    shader->setUniformVector("NodeDataArray", nodesDatas);
    shader->setUniformVector("SourceColorsArray", nodesColors);
    shader->setUniformVector("WrapModesArray", wrapModes);

    modelInfosBuffer->use(0);
    drawInfoBuffer->use(1);

    for (int i = 0; i < textureBinds.size(); i++) {
        textureBinds[i]->use(i);
    }
    if (material->disableFaceCull)glDisable(GL_CULL_FACE);
    info3d->drawInstanced(instancesFiltered);
    if (material->disableFaceCull)glEnable(GL_CULL_FACE);
}

void Mesh3dLodLevel::setUniforms()
{
    samplerIndices.clear();
    modes.clear();
    targets.clear();
    sources.clear();
    modifiers.clear();
    uvScales.clear();
    nodesDatas.clear();
    nodesColors.clear();
    modes.clear();
    textureBinds.clear();
    wrapModes.clear();

    int samplerIndex = 0;
    useGeometryShader = false;
    for (int i = 0; i < material->nodes.size(); i++) {
        MaterialNode * node = material->nodes[i];
        if (node->target == NODE_TARGET_DISPLACEMENT) useGeometryShader = true;
        samplerIndices.push_back(samplerIndex);
        modes.push_back(node->mixingMode);
        targets.push_back(node->target);
        sources.push_back(node->source);
        modifiers.push_back(node->modifierflags);
        uvScales.push_back(node->uvScale);
        nodesDatas.push_back(node->data);
        nodesColors.push_back(node->color);
        wrapModes.push_back(node->wrap);
        if (node->texture != nullptr && node->source == NODE_SOURCE_TEXTURE) {
            textureBinds.push_back(node->texture);
            samplerIndex++;
        }
    }
}

void Mesh3dLodLevel::updateBuffer(const vector<Mesh3dInstance*> &instances)
{
    vec3 cameraPos = Game::instance->world->mainDisplayCamera->transformation->position;
    vector<Mesh3dInstance*> filtered;
    for (int i = 0; i < instances.size(); i++) {
        float dst = distance(cameraPos, instances[i]->transformation->position);
        if (dst >= distanceStart && dst < distanceEnd) {
            filtered.push_back(instances[i]);
        }
    }
    instancesFiltered = filtered.size();
    /*layout rotation f4 translation f3+1 scale f3+1 =>> 12 floats*/
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