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
private:
    bool isFreeSpace(VkDeviceSize offset, VkDeviceSize size);
    VkDeviceMemory handle;
    VulkanToolkit* vulkan;
    uint32_t type;
    VkDeviceSize chunkSize;
    std::vector<VulkanSingleAllocation> allActiveAllocations;
    VkDeviceSize allAllocationsSize;
};

struct VulkanSingleAllocation {
public:
    VulkanSingleAllocation(VulkanMemoryChunk* ichunk, VkDeviceSize isize, VkDeviceSize ioffset)
        : chunk(ichunk), size(isize), offset(ioffset) {}
    VulkanMemoryChunk* chunk;
    VkDeviceSize size;
    VkDeviceSize offset;
    void free() {
        chunk->freeBindMemory(VulkanSingleAllocation(chunk, size, offset));
    }
};