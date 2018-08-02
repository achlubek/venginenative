#pragma once
class Object3dInfo;
class VulkanDevice;
class Object3dInfoFactory
{
public:
    Object3dInfoFactory(VulkanDevice* device);
    ~Object3dInfoFactory();

    Object3dInfo* build(std::string mediakey);
    Object3dInfo* build(std::vector<float> rawData);
    Object3dInfo* getFullScreenQuad();
private:
    VulkanDevice * device;
    Object3dInfo* fullScreenQuad;
};

