#pragma once
class Mesh3d;
class Material;
class Object3dInfo;
class Mesh3dInstance;

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
    bool ignoreFrustumCulling = false; 
    void draw(VulkanRenderStage* stage, const Mesh3d* mesh);
    void updateBuffer(const Mesh3d* mesh, glm::mat4 transform, const vector<Mesh3dInstance*> &instances);
    void initialize(); 
    VulkanDescriptorSet* descriptorSet;
private:
    VulkanGenericBuffer *modelInfosBuffer;

    bool checkIntersection(glm::mat4 transform, Mesh3dInstance* instance);

    size_t instancesFiltered;
    vector<unsigned int> lastIdMap = vector<unsigned int>{};
    vector<unsigned int> newids = vector<unsigned int>{};
};
