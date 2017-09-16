#pragma once
class Mesh3dInstance;
class Mesh3dLodLevel;
class Object3dInfo;

#include "Material.h";
class Mesh3d
{
public:
    Mesh3d();
    ~Mesh3d();
    static Mesh3d *create(Object3dInfo *info, Material *material);
    void addInstance(Mesh3dInstance *instance);
    void addLodLevel(Mesh3dLodLevel *level);
    void clearInstances();
    void clearLodLevels();
    vector<Mesh3dInstance*>& getInstances();
    vector<Mesh3dLodLevel*>& getLodLevels();
    Mesh3dInstance *getInstance(int index);
    Mesh3dLodLevel *getLodLevel(int index);
    void removeInstance(Mesh3dInstance* instance);
    void removeLodLevel(Mesh3dLodLevel* level);
    void updateBuffers(glm::mat4 transform);
    void draw(VulkanRenderStage* stage);
    void setUniforms(glm::mat4 transform);
    unsigned int id;
    string name = "";
    int lastUpdateFrameId;
    bool selectable = true;
    bool visible = true;
    bool castShadow = true;
private:
    vector<Mesh3dInstance*> instances;
    vector<Mesh3dLodLevel*> lodLevels;
};
