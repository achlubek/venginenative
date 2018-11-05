#include "stdafx.h"
#include "VulkanMemoryManager.h"
#include "VulkanMemoryChunk.h"
#include "VulkanDevice.h"
#include "VulkanSingleAllocation.h"
namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {

            VulkanMemoryManager::VulkanMemoryManager(VulkanDevice* device)
                : device(device)
            {
                allAllocationsByType = {};
            }


            VulkanMemoryManager::~VulkanMemoryManager()
            {
            }

            VulkanSingleAllocation VulkanMemoryManager::bindBufferMemory(uint32_t type, VkBuffer buffer, VkDeviceSize size)
            {
                size += 0x1000 - (size % 0x1000);
                if (allAllocationsByType.find(type) == allAllocationsByType.end()) {
                    allAllocationsByType[type] = { new VulkanMemoryChunk(device, type) };
                }
                auto chunks = allAllocationsByType.at(type);
                for (int i = 0; i < chunks.size(); i++) {
                    VkDeviceSize offset;
                    bool result = chunks[i]->findFreeMemoryOffset(size, offset);
                    if (result) {
                        return chunks[i]->bindBufferMemory(buffer, size, offset);
                    }
                }
                // no suitable chunk found, create new, add, and alloc
                auto newchunk = new VulkanMemoryChunk(device, type);
                allAllocationsByType[type].push_back(newchunk);
                VkDeviceSize offset;
                bool result = newchunk->findFreeMemoryOffset(size, offset);
                return newchunk->bindBufferMemory(buffer, size, offset);
            }

            VulkanSingleAllocation VulkanMemoryManager::bindImageMemory(uint32_t type, VkImage image, VkDeviceSize size)
            {
                size += 0x1000 - (size % 0x1000);
                if (allAllocationsByType.find(type) == allAllocationsByType.end()) {
                    allAllocationsByType[type] = { new VulkanMemoryChunk(device, type) };
                }
                auto chunks = allAllocationsByType.at(type);
                for (int i = 0; i < chunks.size(); i++) {
                    VkDeviceSize offset;
                    bool result = chunks[i]->findFreeMemoryOffset(size, offset);
                    if (result) {
                        return chunks[i]->bindImageMemory(image, size, offset);
                    }
                }
                // no suitable chunk found, create new, add, and alloc
                auto newchunk = new VulkanMemoryChunk(device, type);
                allAllocationsByType[type].push_back(newchunk);
                VkDeviceSize offset;
                bool result = newchunk->findFreeMemoryOffset(size, offset);
                return newchunk->bindImageMemory(image, size, offset);
            }

            uint64_t VulkanMemoryManager::getTotalAllocatedMemory()
            {
                uint64_t sum = 0;
                for (auto const& element : allAllocationsByType) {
                    for (auto const& chunk : element.second) {
                        sum += chunk->getTotalAllocatedMemory();
                    }
                }
                return sum;
            }
        }
    }
}