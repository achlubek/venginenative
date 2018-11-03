#pragma once
namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {

            class VulkanMemoryChunk;
            class VulkanSingleAllocation;
            class VulkanDevice;
            class VulkanMemoryManager
            {
            public:
                VulkanMemoryManager(VulkanDevice* device);
                ~VulkanMemoryManager();

                VulkanDevice* device;

                VulkanSingleAllocation bindBufferMemory(uint32_t type, VkBuffer buffer, VkDeviceSize size);
                VulkanSingleAllocation bindImageMemory(uint32_t type, VkImage image, VkDeviceSize size);

                uint64_t getTotalAllocatedMemory();

            private:
                std::map<uint32_t, std::vector<VulkanMemoryChunk*>> allAllocationsByType;
            };

        }
    }
}