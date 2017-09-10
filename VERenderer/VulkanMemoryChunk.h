#pragma once
struct VulkanSingleAllocation;
class VulkanMemoryChunk
{
public:
    VulkanMemoryChunk(VulkanToolkit* vulkan, uint32_t type);
    ~VulkanMemoryChunk();

    VulkanSingleAllocation bindBufferMemory(VkBuffer buffer, VkDeviceSize size, VkDeviceSize offset);
    VulkanSingleAllocation bindImageMemory(VkImage image, VkDeviceSize size, VkDeviceSize offset);
    void freeBindMemory(VulkanSingleAllocation allocation);
    bool findFreeMemoryOffset(VkDeviceSize size, VkDeviceSize &outOffset);
    VkDeviceMemory handle;
private:
    bool isFreeSpace(VkDeviceSize offset, VkDeviceSize size);
    VulkanToolkit* vulkan;
    uint32_t type;
    VkDeviceSize chunkSize;
    std::vector<VulkanSingleAllocation> allActiveAllocations;
    VkDeviceSize allAllocationsSize;
    volatile bool inUse = false;
};

struct VulkanSingleAllocation {
public:
    VulkanSingleAllocation() {}
    VulkanSingleAllocation(VulkanMemoryChunk* ichunk, VkDeviceSize isize, VkDeviceSize ioffset)
        : chunk(ichunk), size(isize), offset(ioffset) {}
    VulkanMemoryChunk* chunk;
    VkDeviceSize size;
    VkDeviceSize offset;
    void free() {
        chunk->freeBindMemory(VulkanSingleAllocation(chunk, size, offset));
    }
};