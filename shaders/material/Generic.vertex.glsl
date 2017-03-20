#version 430 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec4 in_tangent;

out Data {
#include InOutStageLayout.glsl
} Output;

#include Mesh3dUniforms.glsl
#include Quaternions.glsl

flat out uint MeshInstanceID;
flat out uint LodLevelID;
flat out uint MeshID;
layout (std430, binding = 1) buffer MatBuffer
{
    vec4 RoughnessMetalness_ZeroZero;
    vec4 DiffuseColor_Zero;
    ivec4 UseTex_DNBR;
    ivec4 UseTex_M_UseSkeleton_Zero_Zero;
    vec4 ScaleTex_DN;
    vec4 ScaleTex_BR;
    vec4 ScaleTex_MZeroZero;
    uvec4 MeshId_LodLevelId_ZeroZero;
};

struct SkeletonWeightsStruct{
	int weights_count;
	int[7] bones;
	float[8] weights;
};
layout (std430, binding = 2) buffer SkelWeiBuffer
{
    SkeletonWeightsStruct[] skeletonWeightsArray;
};
layout (std430, binding = 3) buffer SkelPosBuffer
{
    mat4[] skeletonPose;
};
vec3 applySkeletonPose(vec4 v){
    int id = gl_VertexID;
    SkeletonWeightsStruct sws = skeletonWeightsArray[id];
    float w = 0.0;
    vec3 result = vec3(0.0);
    for(int i=0;i<sws.weights_count;i++){
        float wei = sws.weights[i];
        result += wei * ((skeletonPose[sws.bones[i]] * v).xyz);
        w += wei;
    }
    result /= max(0.01, w);
    float stp = step( 0.001, w);
    return result * stp;// + v * (1.0 - stp);
}

#include ModelBuffer.glsl

void main(){

    vec4 v = vec4(in_position,1);
    if(UseTex_M_UseSkeleton_Zero_Zero.y == 1) v.xyz = applySkeletonPose(v);
    vec3 inorm = in_normal;
    if(UseTex_M_UseSkeleton_Zero_Zero.y == 1) inorm = normalize(applySkeletonPose(vec4(inorm, 0.0)));

    Output.instanceId = int(gl_InstanceID);
    Output.TexCoord = vec2(in_uv.x, in_uv.y);
    Output.WorldPos = transform_vertex(int(gl_InstanceID), v.xyz);
    Output.Normal = normalize(transform_normal(int(gl_InstanceID), inorm));
    Output.Tangent = clamp(vec4(normalize(transform_normal(int(gl_InstanceID), in_tangent.xyz)), in_tangent.w), -1.0, 1.0);
    vec4 outpoint = (VPMatrix) * vec4(Output.WorldPos, 1.0);
//    outpoint.w = 0.5 + 0.5 * outpoint.w;
    //outpoint.w = - outpoint.w;
    MeshInstanceID = ModelInfos[int(gl_InstanceID)].idAnd4Empty.x;
    LodLevelID = MeshId_LodLevelId_ZeroZero.y;
    MeshID = MeshId_LodLevelId_ZeroZero.x;
    Output.Data.x = 1.0;
    Output.Data.y =  ( outpoint.z ) * 0.5 + 0.5 ;
    gl_Position = outpoint;// + vec4(0, 0.9, 0, 0);
}
