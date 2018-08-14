#include "stdafx.h"
#include "VulkanSingleAllocation.h"
#include "VulkanMemoryChunk.h"
namespace VEngine
{
    namespace Renderer
    {
        namespace Internal
        {

            VulkanSingleAllocation::VulkanSingleAllocation()
            {
            }

            VulkanSingleAllocation::VulkanSingleAllocation(VulkanMemoryChunk * ichunk, uint64_t isize, uint64_t ioffset)
                : chunk(ichunk), size(isize), offset(ioffset)
            {
            }


            VulkanSingleAllocation::~VulkanSingleAllocation()
            {
            }

            void VulkanSingleAllocation::free()
            {
                chunk->freeBindMemory(VulkanSingleAllocation(chunk, size, offset));
            }

            void VulkanSingleAllocation::map(uint64_t ioffset, uint64_t isize, void ** data)
            {
                vkMapMemory(chunk->getDevice(), chunk->getHandle(), ioffset + offset, isize, 0, data);
            }

            void VulkanSingleAllocation::map(void ** data)
            {
                vkMapMemory(chunk->getDevice(), chunk->getHandle(), offset, size, 0, data);
            }

            void VulkanSingleAllocation::unmap()
            {
                vkUnmapMemory(chunk->getDevice(), chunk->getHandle());
            }
        }
    }
}