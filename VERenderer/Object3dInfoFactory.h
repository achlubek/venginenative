#pragma once
class Object3dInfo;
class VulkanToolkit;
class Object3dInfoFactory
{
public:
    Object3dInfoFactory(VulkanToolkit* vulkan);
    ~Object3dInfoFactory();

    Object3dInfo* build(std::string mediakey);
    Object3dInfo* build(std::vector<float> rawData);
private:
    VulkanToolkit * vulkan;
};

