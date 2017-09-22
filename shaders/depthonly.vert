#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

#include sharedBuffers.glsl

struct ModelInfo{
    mat4 Transformation;
    uvec4 idAnd4Empty;
};
layout(set = 1, binding = 0) buffer UniformBufferObject3 {
    ModelInfo ModelInfos[];
} modelData;

layout(location = 0) out vec3 outWorldPos;

layout(location = 0) in vec3 inPosition;
void main() {

    outWorldPos = (modelData.ModelInfos[int(gl_InstanceIndex)].Transformation
        * vec4(inPosition.xyz, 1.0)).rgb;
    vec4 opo = (hiFreq.VPMatrix)
        * modelData.ModelInfos[int(gl_InstanceIndex)].Transformation
        * vec4(inPosition.xyz, 1.0);
    gl_Position = opo;
}
