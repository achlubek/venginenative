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

void VulkanBinaryBufferBuilder::emplaceGeneric(unsigned char* m, int bytes)
{ 
    for (int i = 0; i < bytes; i++) emplaceByte(*(m + i));
} 
