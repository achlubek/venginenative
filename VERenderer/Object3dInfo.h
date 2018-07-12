#pragma once
class VulkanDevice;
class VulkanVertexBuffer;

class Object3dInfo
{
public:
    Object3dInfo(VulkanDevice * device, std::vector<float> &vbo);
    ~Object3dInfo();

    VulkanVertexBuffer* getVertexBuffer();

private: 
    VulkanVertexBuffer* vertexBuffer;
};
