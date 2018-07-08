#include "stdafx.h"
#include "Object3dInfo.h" 
#include "VulkanToolkit.h" 
#include "VulkanDescriptorSet.h" 
#include "Internal/VulkanCommandBuffer.h" 
#include "Internal/VulkanMemoryManager.h" 
#include "Internal/VulkanMemoryChunk.h" 
#include "Internal/VulkanVertexBuffer.h" 
#include <vulkan.h>

/*
This class expected interleaved buffer in format
pos.xyz-uv.xy-normal.xyz-tangent.xyzw
totals in 12 elements per vertex
*/

Object3dInfo::Object3dInfo(VulkanToolkit * vulkan, std::vector<float> &vboin)
    : vulkan(vulkan)
{
    vertexBuffer = new VulkanVertexBuffer(vulkan, vboin);
}

Object3dInfo::~Object3dInfo()
{
    delete vertexBuffer;
}

VulkanVertexBuffer * Object3dInfo::getVertexBuffer()
{
    return vertexBuffer;
}
