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

layout(set = 2, binding = 0) uniform UniformBufferObject6 {
    vec4 RoughnessMetalness_ZeroZero;
    vec4 DiffuseColor_Zero;
    ivec4 UseTex_DNBR;
    ivec4 UseTex_M_UseSkeleton_Zero_Zero;
    vec4 ScaleTex_DN;
    vec4 ScaleTex_BR;
    vec4 ScaleTex_MZeroZero;
    uvec4 MeshId_LodLevelId_ZeroZero;
} materialData;

vec3 transform_normal(int info, vec3 normal){
    ModelInfo o = modelData.ModelInfos[info];
    return mat3(o.Transformation) * normal;
}


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec4 outTangent;
layout(location = 2) out vec2 outTexCoord;
layout(location = 3) out flat uint inInstanceId;
layout(location = 4) out vec3 outWorldPos;


void main() {

    vec3 WorldPos = (modelData.ModelInfos[int(gl_InstanceIndex)].Transformation
        * vec4(inPosition.xyz, 1.0)).rgb;
    vec4 opo = (hiFreq.VPMatrix)
        * modelData.ModelInfos[int(gl_InstanceIndex)].Transformation
        * vec4(inPosition.xyz, 1.0);
    vec3 Normal = inNormal;
    outNormal = normalize(Normal);
    outTangent = clamp(vec4(normalize(transform_normal(int(gl_InstanceIndex), inTangent.xyz)), inTangent.w), -1.0, 1.0);
    outTexCoord = inTexCoord;
    inInstanceId = gl_InstanceIndex;
    outWorldPos = WorldPos;
    //opo.y *= -1.0;
    gl_Position = opo;
    //outDepth = 1.0 - ((opo.z/opo.w) * 0.5 + 0.5);
}
