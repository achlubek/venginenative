#pragma once
class Mesh3d;
#include "Material.h"
#include "Object3dInfo.h"
#include "Mesh3dInstance.h"
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
    bool disableFaceCulling = true;
    bool visible = true;
    bool ignoreFrustumCulling = false;
    bool castShadow = true;
    bool selectable = true;
    void draw(VulkanRenderStage* stage, const Mesh3d* mesh);
    void updateBuffer(const Mesh3d* mesh, const vector<Mesh3dInstance*> &instances);
    bool materialBufferNeedsUpdate = true;
	VulkanDescriptorSet descriptorSet;
	void initialize();
	//Skeleton* skeleton;
	//SkeletonPose* skeletonPose;
private:
    VulkanGenericBuffer *modelInfosBuffer;

	VulkanGenericBuffer *materialBuffer;
    
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
    vector<VulkanImage*> textureBinds;
    size_t instancesFiltered;
    vector<int> lastIdMap = vector<int>{}; 
};
