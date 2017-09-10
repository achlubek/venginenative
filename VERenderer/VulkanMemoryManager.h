#pragma once
#include <map>
class VulkanMemoryChunk;
struct VulkanSingleAllocation;
class VulkanToolkit;
class VulkanMemoryManager
{
public:
    VulkanMemoryManager(VulkanToolkit* vulkan);
    ~VulkanMemoryManager();

    VulkanToolkit* vulkan;

    VulkanSingleAllocation bindBufferMemory(uint32_t type, VkBuffer buffer, VkDeviceSize size);
    VulkanSingleAllocation bindImageMemory(uint32_t type, VkImage image, VkDeviceSize size);

    std::map<uint32_t, std::vector<VulkanMemoryChunk*>> allAllocationsByType;
};

