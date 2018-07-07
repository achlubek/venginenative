#pragma once
struct VulkanSingleAllocation;
class VulkanMemoryChunk
{
public:
    VulkanMemoryChunk(VulkanToolkit* vulkan, uint32_t type);
    ~VulkanMemoryChunk();

    VulkanSingleAllocation bindBufferMemory(VkBuffer buffer, uint64_t size, uint64_t offset);
    VulkanSingleAllocation bindImageMemory(VkImage image, uint64_t size, uint64_t offset);
    void freeBindMemory(VulkanSingleAllocation allocation);
    bool findFreeMemoryOffset(uint64_t size, uint64_t &outOffset);
    VkDeviceMemory getHandle();
    VulkanToolkit* getVulkanToolkit();
private:
    bool isFreeSpace(uint64_t offset, uint64_t size);
    VulkanToolkit* vulkan;
    uint32_t type;
    uint64_t chunkSize;
    std::vector<VulkanSingleAllocation> allActiveAllocations;
    volatile bool inUse = false;
    VkDeviceMemory handle;
    uint64_t allAllocationsSize;
};
