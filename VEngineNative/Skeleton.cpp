#include "stdafx.h"
#include "Skeleton.h"


Skeleton::Skeleton()
{ 
	weightsSSBO = new ShaderStorageBuffer();
	bonesSSBO = new ShaderStorageBuffer();
}


Skeleton::~Skeleton()
{
}

/*
SSBO weights LAYOUT
struct{
	int weights_count;
	int[7] bones;
	float[8] weights;
}
chunk into 8 * 4 + 8 * 4 = 64 per element
thats a much btw

SSBO3
mat4 bones array
*/
void Skeleton::updateBuffers(SkeletonPose* pose, bool poseOnly)
{
	if (!poseOnly) {
		auto pointerI = (GLint*)weightsSSBO->acquireAsynchronousPointer(0, 64 * weights.size());
		auto pointerF = (GLfloat*)pointerI; // uhh RISKY!
		int cursor = 0;
		for (int i = 0; i < weights.size(); i++) {
			auto elem = weights[i];
			int wcnt = elem.size();
			pointerI[cursor++] = wcnt;

			pointerI[cursor++] = wcnt > 0 ? elem[0].bone : 0;
			pointerI[cursor++] = wcnt > 1 ? elem[1].bone : 0;
			pointerI[cursor++] = wcnt > 2 ? elem[2].bone : 0;

			pointerI[cursor++] = wcnt > 3 ? elem[3].bone : 0;
			pointerI[cursor++] = wcnt > 4 ? elem[4].bone : 0;
			pointerI[cursor++] = wcnt > 5 ? elem[5].bone : 0;
			pointerI[cursor++] = wcnt > 6 ? elem[6].bone : 0; // 7th

			pointerF[cursor++] = wcnt > 0 ? elem[0].weight : 0.0f;
			pointerF[cursor++] = wcnt > 1 ? elem[1].weight : 0.0f;
			pointerF[cursor++] = wcnt > 2 ? elem[2].weight : 0.0f;
			pointerF[cursor++] = wcnt > 3 ? elem[3].weight : 0.0f;

			pointerF[cursor++] = wcnt > 4 ? elem[4].weight : 0.0f;
			pointerF[cursor++] = wcnt > 5 ? elem[5].weight : 0.0f;
			pointerF[cursor++] = wcnt > 6 ? elem[6].weight : 0.0f; // 7th
			pointerF[cursor++] = 0; // 8th
		}
		weightsSSBO->unmapBuffer();
		auto pointer = (GLfloat*)bonesSSBO->acquireAsynchronousPointer(0, sizeof(GLfloat) * 4 * 4 * pose->pose.size());
		cursor = 0;
		for (unsigned int i = 0; i < pose->pose.size(); i++) {
			glm::mat4 m = pose->pose[i];
			for (unsigned int d = 0; d < 4; d++) {
				pointer[cursor++] = m[d].x;
				pointer[cursor++] = m[d].y;
				pointer[cursor++] = m[d].z;
				pointer[cursor++] = m[d].w;
			}
		}
		bonesSSBO->unmapBuffer();
	}
}

void Skeleton::use(SkeletonPose* pose, int weightIndex, int bonesIndex)
{
	if (needsUpdate) {
		updateBuffers(pose, false);
	}
	weightsSSBO->use(weightIndex);
	bonesSSBO->use(bonesIndex);

}
