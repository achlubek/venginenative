#include "stdafx.h"
#include "Skeleton.h"


Skeleton::Skeleton()
{
	pointersSSBO = new ShaderStorageBuffer();
	weightsSSBO = new ShaderStorageBuffer();
	bonesSSBO = new ShaderStorageBuffer();
}


Skeleton::~Skeleton()
{
}

/*
SSBO 1 layout
struct{int pointer, weightcount, 0 ,0}
it stores pointers and count of weights, inteleaved, indexed by vertex id, to bone weight array

SSBO 2
{struct bone index, weight,0,0}
pointers to this in ssbo 1!

SSBO3
mat4 bones array
*/
void Skeleton::updateBuffers()
{
}
