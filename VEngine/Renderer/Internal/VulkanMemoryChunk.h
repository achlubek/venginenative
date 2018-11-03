#pragma once

namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {

            class VulkanSingleAllocation;
            class VulkanDevice;
            class VulkanMemoryChunk
            {
            public:
                VulkanMemoryChunk(VulkanDevice* device, uint32_t type);
                ~VulkanMemoryChunk();

                VulkanSingleAllocation bindBufferMemory(VkBuffer buffer, uint64_t size, uint64_t offset);
                VulkanSingleAllocation bindImageMemory(VkImage image, uint64_t size, uint64_t offset);
                void freeBindMemory(VulkanSingleAllocation allocation);
                bool findFreeMemoryOffset(uint64_t size, uint64_t &outOffset);
                VkDeviceMemory getHandle();
                VkDevice getDevice();
                uint64_t getTotalAllocatedMemory();
            private:
                bool isFreeSpace(uint64_t offset, uint64_t size);
                VulkanDevice* device;
                uint32_t type;
                uint64_t chunkSize;
                std::vector<VulkanSingleAllocation> allActiveAllocations;
                volatile bool inUse = false;
                VkDeviceMemory handle;
                uint64_t allAllocationsSize;
            };
        }
    }
}