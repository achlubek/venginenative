#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(set = 0, binding = 0) uniform UniformBufferObject1 {
    float Time;
    float Zero;
    vec2 Mouse;
    mat4 VPMatrix;
    vec4 inCameraPos;
    vec4 inFrustumConeLeftBottom;
    vec4 inFrustumConeBottomLeftToBottomRight;
    vec4 inFrustumConeBottomLeftToTopLeft;
} hiFreq;

#define CameraPosition (hiFreq.inCameraPos.xyz)
#define FrustumConeLeftBottom (hiFreq.inFrustumConeLeftBottom.xyz)
#define FrustumConeBottomLeftToBottomRight (hiFreq.inFrustumConeBottomLeftToBottomRight.xyz)
#define FrustumConeBottomLeftToTopLeft (hiFreq.inFrustumConeBottomLeftToTopLeft.xyz)

layout(set = 0, binding = 1) uniform UniformBufferObject2 {
    float Time;
    float Zero;
    vec2 Mouse;
    mat4 VPMatrix;
} lowFreq;

struct ModelInfo{
    vec4 Rotation;
    vec4 Translation;
    vec4 Scale;
    uvec4 idAnd4Empty;
};
layout(set = 0, binding = 2) buffer UniformBufferObject3 {
    ModelInfo ModelInfos[];
} modelData;
layout(set = 0, binding = 3) uniform UniformBufferObject6 {
    vec4 RoughnessMetalness_ZeroZero;
    vec4 DiffuseColor_Zero;
    ivec4 UseTex_DNBR;
    ivec4 UseTex_M_UseSkeleton_Zero_Zero;
    vec4 ScaleTex_DN;
    vec4 ScaleTex_BR;
    vec4 ScaleTex_MZeroZero;
    uvec4 MeshId_LodLevelId_ZeroZero;
} materialData;
vec3 quat_mul_vec( vec4 q, vec3 v ){
    return v + 2.0*cross(cross(v, q.xyz ) + q.w*v, q.xyz);
}

vec3 transform_vertex(int info, vec3 vertex){
    vec3 result = vertex;
    ModelInfo o = modelData.ModelInfos[info];
    result *= o.Scale.xyz;
    result = quat_mul_vec(o.Rotation, result);
    result += o.Translation.xyz;
    return result;
}

vec3 transform_normal(int info, vec3 normal){
    vec3 result = normal;
    ModelInfo o = modelData.ModelInfos[info];
    result = quat_mul_vec(o.Rotation, result);
    result *= 1.0 / o.Scale.xyz;
    return result;
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

    vec3 WorldPos = transform_vertex(int(gl_InstanceIndex), inPosition.xyz);
    vec4 opo = (hiFreq.VPMatrix) * vec4(WorldPos, 1.0);
    vec3 Normal = normalize((int(gl_InstanceIndex), (inNormal)));
    outNormal = Normal;
    outTangent = clamp(vec4(normalize(transform_normal(int(gl_InstanceIndex), inTangent.xyz)), inTangent.w), -1.0, 1.0);
    outTexCoord = inTexCoord;
    inInstanceId = gl_InstanceIndex;
    outWorldPos = WorldPos;
    //opo.y *= -1.0;
    gl_Position = opo;
    //outDepth = 1.0 - ((opo.z/opo.w) * 0.5 + 0.5);
}
