#include "stdafx.h"
#include "VulkanSingleAllocation.h"
#include "VulkanMemoryChunk.h"


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

uint64_t VulkanSingleAllocation::getOffset()
{
    return offset;
}

uint64_t VulkanSingleAllocation::getSize()
{
    return size;
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