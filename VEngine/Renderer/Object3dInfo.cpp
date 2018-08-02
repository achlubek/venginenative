#include "stdafx.h"
#include "Object3dInfo.h"
class VulkanDevice;
#include "Internal/VulkanVertexBuffer.h"

/*
This class expected interleaved buffer in format
pos.xyz-uv.xy-normal.xyz-tangent.xyzw
totals in 12 elements per vertex
*/

Object3dInfo::Object3dInfo(VulkanDevice * device, std::vector<float> &vboin)
{
    vertexBuffer = new VulkanVertexBuffer(device, vboin);
}

Object3dInfo::~Object3dInfo()
{
    delete vertexBuffer;
}

VulkanVertexBuffer * Object3dInfo::getVertexBuffer()
{
    return vertexBuffer;
}
