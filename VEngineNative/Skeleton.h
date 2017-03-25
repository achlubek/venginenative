#pragma once
#include "ShaderStorageBuffer.h"

// framework for this will be...
// ...barebone

struct SkeletonBoneWeight {
public:
	int bone;
	float weight;
	SkeletonBoneWeight(int b, float w) {
		bone = b;
		weight = w;
	}
};

class SkeletonPose {
public:
	vector<glm::mat4> pose;
};

class SkeletonAnimation {
	vector<SkeletonPose*> poses;
	vector<float> times;
};

class Skeleton
{
public:
	Skeleton();
	~Skeleton();
	vector<vector<SkeletonBoneWeight>> weights; // list of "vertices" each containing a list of "weights"
    vector<glm::vec3> positions;
    vector<float> radiues;
    vector<int> parents; 
	ShaderStorageBuffer* weightsSSBO;
	ShaderStorageBuffer* bonesSSBO;
	void updateBuffers(SkeletonPose* pose, bool poseOnly);
	void use(SkeletonPose* pose, int weightIndex, int bonesIndex);
    volatile bool needsUpdate = true;
    volatile bool poseNeedsUpdate = true;
};

