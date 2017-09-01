#pragma once 
class Mesh3d;
class Scene
{
public:
    Scene();
    ~Scene();
    void draw(VulkanRenderStage *stage);
    void prepareFrame();
    void addMesh3d(Mesh3d *item);
    vector<Mesh3d*>& getMesh3ds();
private:
    vector<Mesh3d*> mesh3ds;
};
