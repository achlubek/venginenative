#pragma once
#include <map>
#include <vulkan.h>
class VulkanMemoryChunk;
struct VulkanSingleAllocation;
class VulkanDevice;
class VulkanMemoryManager
{
public:
    VulkanMemoryManager(VulkanDevice* device);
    ~VulkanMemoryManager();

    VulkanDevice* device;

    VulkanSingleAllocation bindBufferMemory(uint32_t type, VkBuffer buffer, VkDeviceSize size);
    VulkanSingleAllocation bindImageMemory(uint32_t type, VkImage image, VkDeviceSize size);

    std::map<uint32_t, std::vector<VulkanMemoryChunk*>> allAllocationsByType;
};

