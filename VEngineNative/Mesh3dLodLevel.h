#pragma once
class Mesh3d;
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
    unsigned int id;
    bool visible = true;
    bool castShadow = true;
    bool selectable = true;
    void draw(const Mesh3d* mesh);
    void updateBuffer(const Mesh3d* mesh, const vector<Mesh3dInstance*> &instances);
    bool materialBufferNeedsUpdate = true;
private:
    ShaderStorageBuffer *modelInfosBuffer1;
    ShaderStorageBuffer *modelInfosBuffer2;
    ShaderStorageBuffer *modelInfosBuffer3;

    ShaderStorageBuffer *materialBuffer;
    
    int currentBuffer = 0;
    int nextBuffer = 1; //smart
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
    size_t instancesFiltered1;
    size_t instancesFiltered2;
    size_t instancesFiltered3;
    vector<int> lastIdMap = vector<int>{};
    int pendingUpdates = 0;
};
