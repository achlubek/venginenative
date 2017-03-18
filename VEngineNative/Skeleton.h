#pragma once
#include "ShaderStorageBuffer.h"

struct SkeletonBoneWeight {
public:
	int bone;
	float weight;
	SkeletonBoneWeight(int b, float w) {
		bone = b;
		weight = w;
	}
};

class Skeleton
{
public:
	Skeleton();
	~Skeleton();
	vector<vector<SkeletonBoneWeight>> weights; // list of "vertices" each containing a list of "weights"
	vector<glm::mat4> bones;
	ShaderStorageBuffer* pointersSSBO;
	ShaderStorageBuffer* weightsSSBO;
	ShaderStorageBuffer* bonesSSBO;
	void updateBuffers();
};

