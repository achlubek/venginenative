#pragma once
#include "ShaderStorageBuffer.h"
#include "Material.h"
#include "Object3dInfo.h"
#include "Mesh3dInstance.h"
#include "Texture2d.h"
class Mesh3dLodLevel
{
public:
    Mesh3dLodLevel(Object3dInfo *info, Material *imaterial, float distancestart, float distanceend);
    Mesh3dLodLevel(Object3dInfo *info, Material *imaterial);
    Mesh3dLodLevel();
    ~Mesh3dLodLevel();
    Material *material;
    Object3dInfo *info3d;
    float distanceStart;
    float distanceEnd;
    float needBufferUpdate = true;
    void draw();
    void setUniforms();
    void updateBuffer(const vector<Mesh3dInstance*> &instances);
    ShaderStorageBuffer *modelInfosBuffer;
private:
    bool checkIntersection(Mesh3dInstance* instance);
    vector<int> samplerIndices;
    vector<int> modes;
    vector<int> targets;
    vector<int> sources;
    vector<int> modifiers;
    vector<int> wrapModes;
    vector<glm::vec2> uvScales;
    vector<glm::vec4> nodesDatas;
    vector<glm::vec4> nodesColors;
    vector<Texture2d*> textureBinds;
    size_t instancesFiltered;
};
