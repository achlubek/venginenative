#include "stdafx.h"

VulkanBinaryBufferBuilder::VulkanBinaryBufferBuilder()
{
    buffer = std::vector<unsigned char>();
}


VulkanBinaryBufferBuilder::~VulkanBinaryBufferBuilder()
{
    
}

void * VulkanBinaryBufferBuilder::getPointer()
{
    return buffer.data();
}

void VulkanBinaryBufferBuilder::emplaceByte(unsigned char byte)
{
    buffer.push_back(byte);
}

void VulkanBinaryBufferBuilder::emplaceInt32(int32_t d)
{
    unsigned char* data = (unsigned char*)&d;
    for (int i = 0; i < 4; i++) emplaceByte(*(data + i));
}

void VulkanBinaryBufferBuilder::emplaceFloat32(float d)
{
    unsigned char* data = (unsigned char*)&d;
    for (int i = 0; i < 4; i++) emplaceByte(*(data + i));
}

void VulkanBinaryBufferBuilder::emplaceMat4(glm::mat4 m)
{
    unsigned char* data = (unsigned char*)&m;
    for (int i = 0; i < sizeof(m); i++) emplaceByte(*(data + i));
}

void VulkanBinaryBufferBuilder::emplaceVec3(glm::vec3 v)
{
    emplaceFloat32(v.x);
    emplaceFloat32(v.y);
    emplaceFloat32(v.z);
}
