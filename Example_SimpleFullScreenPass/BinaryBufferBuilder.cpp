#include "stdafx.h"
#include "BinaryBufferBuilder.h"


BinaryBufferBuilder::BinaryBufferBuilder()
{
    buffer = std::vector<unsigned char>();
}


BinaryBufferBuilder::~BinaryBufferBuilder()
{

}

void * BinaryBufferBuilder::getPointer()
{
    return buffer.data();
}

void BinaryBufferBuilder::emplaceByte(unsigned char byte)
{
    buffer.push_back(byte);
}

void BinaryBufferBuilder::emplaceInt32(int32_t d)
{
    unsigned char* data = (unsigned char*)&d;
    for (int i = 0; i < 4; i++) emplaceByte(*(data + i));
}

void BinaryBufferBuilder::emplaceFloat32(float d)
{
    unsigned char* data = (unsigned char*)&d;
    for (int i = 0; i < 4; i++) emplaceByte(*(data + i));
}

void BinaryBufferBuilder::emplaceGeneric(unsigned char* m, int bytes)
{
    for (int i = 0; i < bytes; i++) emplaceByte(*(m + i));
}