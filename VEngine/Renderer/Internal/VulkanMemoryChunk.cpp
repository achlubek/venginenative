#include "stdafx.h"
#include "VulkanMemoryChunk.h"
#include "VulkanSingleAllocation.h"
#include "VulkanDevice.h"
namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {

            VulkanMemoryChunk::VulkanMemoryChunk(VulkanDevice* device, uint32_t itype)
                : device(device), type(itype)
            {
                chunkSize = 256 * 1024 * 1024;
                VkMemoryAllocateInfo allocInfo = {};
                allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocInfo.allocationSize = chunkSize;
                allocInfo.memoryTypeIndex = type;

                VkResult result = vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &handle);
                if (result != VK_SUCCESS) {
                    throw std::runtime_error("failed to allocate requested memory");
                }
            }

            VulkanMemoryChunk::~VulkanMemoryChunk()
            {
                size_t allocscount = allActiveAllocations.size();
                for (int i = 0; i < allocscount; i++) {
                    auto a = allActiveAllocations[i];
                    a.free();
                }
                vkFreeMemory(device->getDevice(), handle, nullptr);
            }

            VulkanSingleAllocation VulkanMemoryChunk::bindBufferMemory(VkBuffer buffer, uint64_t size, uint64_t offset)
            {
                while (inUse); // todo mutex
                inUse = true;
                vkBindBufferMemory(device->getDevice(), buffer, handle, offset);
                auto va = VulkanSingleAllocation(this, size, offset);
                allActiveAllocations.push_back(va);
                allAllocationsSize += size;
                inUse = false;
                return va;
            }

            VulkanSingleAllocation VulkanMemoryChunk::bindImageMemory(VkImage image, uint64_t size, uint64_t offset)
            {
                while (inUse);
                inUse = true;
                vkBindImageMemory(device->getDevice(), image, handle, offset);
                auto va = VulkanSingleAllocation(this, size, offset);
                allActiveAllocations.push_back(va);
                allAllocationsSize += size;
                inUse = false;
                return va;
            }

            void VulkanMemoryChunk::freeBindMemory(VulkanSingleAllocation allocation)
            {
                while (inUse);
                inUse = true;
                size_t allocscount = allActiveAllocations.size();
                for (int i = 0; i < allocscount; i++) {
                    auto a = allActiveAllocations[i];
                    if (a.offset == allocation.offset && a.size == allocation.size) {
                        allActiveAllocations.erase(allActiveAllocations.begin() + i);
                        allAllocationsSize -= allocation.size;
                        break;
                    }
                }
                inUse = false;
            }

            bool VulkanMemoryChunk::findFreeMemoryOffset(uint64_t size, uint64_t &outOffset)
            {
                while (inUse);
                inUse = true;
                if (isFreeSpace(0, size)) {
                    outOffset = 0;
                    inUse = false;
                    return true;
                }
                // try the last one for a small performance boost

                size_t allocscount = allActiveAllocations.size();

                auto a = allActiveAllocations[allocscount - 1];
                if (isFreeSpace(a.offset + a.size + 2048, size)) {
                    outOffset = a.offset + a.size + 2048;
                    inUse = false;
                    return true;
                }

                for (int i = 0; i < allocscount; i++) {
                    auto a = allActiveAllocations[i];
                    if (isFreeSpace(a.offset + a.size + 2048, size)) {
                        outOffset = a.offset + a.size + 2048;
                        inUse = false;
                        return true;
                    }
                }
                inUse = false;
                return false;
            }

            VkDeviceMemory VulkanMemoryChunk::getHandle()
            {
                return handle;
            }

            VkDevice VulkanMemoryChunk::getDevice()
            {
                return device->getDevice();
            }

            bool VulkanMemoryChunk::isFreeSpace(uint64_t offset, uint64_t size)
            {
                VkDeviceSize end = offset + size;
                size_t allocscount = allActiveAllocations.size();
                if (end >= chunkSize) {
                    return false;
                }
                for (size_t i = 0; i < allocscount; i++) {
                    auto a = allActiveAllocations[i];
                    uint64_t aend = a.offset + a.size;
                    if (offset >= a.offset && offset <= aend) { // if start of alloc collides
                        return false;
                    }
                    if (end >= a.offset && end <= aend) { // if end of alloc collides
                        return false;
                    }
                    if (offset <= a.offset && end >= aend) { // if alloc contains element
                        return false;
                    }
                }
                return true;
            }

        }
    }
}