#pragma once 
class Mesh3d;
class Scene : public AbsTransformable
{
public:
    Scene();
    Scene(TransformationManager * transmgr);
    ~Scene();
    void draw(glm::mat4 parentTransform, VulkanRenderStage *stage);
    void prepareFrame(glm::mat4 parentTransform);
    void addMesh3d(Mesh3d *item);
    void addScene(Scene *item);
    vector<Mesh3d*>& getMesh3ds();
    vector<Scene*>& getScenes();
private:
    vector<Mesh3d*> mesh3ds;
    vector<Scene*> scenes;
};
