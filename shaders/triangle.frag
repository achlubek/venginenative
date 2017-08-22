#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in flat uint inInstanceId;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormal;

layout(binding = 4) uniform sampler2D diffuseTexture;
layout(binding = 5) uniform sampler2D normalTexture;

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
void main() {
//	float dt = max(0.0, dot(normalize(inNormal), normalize(vec3(1.0, -1.0, 0.0)))) *0.99 + 0.01;
vec3 diffuse = pow(texture(diffuseTexture, inTexCoord).rgb , vec3(2.2));
vec3 normal = inNormal;//pow(texture(normalTexture, inTexCoord).rgb , vec3(2.2));
 //normal = quat_mul_vec(modelData.ModelInfos[inInstanceId].Rotation, normal);

    outColor = vec4(diffuse, 1.0);
    outNormal =vec4(normalize(normal), 1.0);
//    gl_FragDepth = 1.0 - inDepth;
}
