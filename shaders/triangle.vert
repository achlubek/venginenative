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
} hiFreq;
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
layout(set = 0, binding = 2) uniform UniformBufferObject3 {
    ModelInfo ModelInfos[];
} modelData1;
layout(set = 0, binding = 3) uniform UniformBufferObject4 {
    ModelInfo ModelInfos[];
} modelData2;
layout(set = 0, binding = 4) uniform UniformBufferObject5 {
    ModelInfo ModelInfos[];
} modelData3;
layout(set = 0, binding = 5) uniform UniformBufferObject6 {
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
    ModelInfo o = modelData1.ModelInfos[0]; // TO DO!!!!!!!!!
    result *= o.Scale.xyz;
    result = quat_mul_vec(o.Rotation, result);
    result += o.Translation.xyz;
    return result;
}

vec3 transform_normal(int info, vec3 normal){
    vec3 result = normal;
    result *= 1.0 / modelData1.ModelInfos[0].Scale.xyz;
    return result;
}


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out float outDepth;

mat3 rotationMatrix(vec3 axis, float angle)
{
	axis = normalize(axis);
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;

	return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
	oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
	oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}

void main() {
    uint instance = gl_InstanceIndex;
    vec3 WorldPos = transform_vertex(int(instance), inPosition.xyz);
    vec4 opo = (hiFreq.VPMatrix) * vec4(inPosition.xyz, 1.0);
    vec3 Normal = normalize(transform_normal(int(instance), normalize(inNormal)));
    opo.y *= -1.0;
    gl_Position = opo;
    outNormal = Normal;
    outTexCoord = inTexCoord;
    outDepth = 1.0 - ((opo.z/opo.w) * 0.5 + 0.5);
}
