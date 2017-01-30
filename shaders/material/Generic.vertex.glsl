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

#include ModelBuffer.glsl

void main(){

    vec4 v = vec4(in_position,1);

    Output.instanceId = int(gl_InstanceID);
    Output.TexCoord = vec2(in_uv.x, in_uv.y);
    Output.WorldPos = transform_vertex(int(gl_InstanceID), v.xyz);
    Output.Normal = normalize(transform_normal(int(gl_InstanceID), in_normal));
    Output.Tangent = clamp(vec4(normalize(transform_normal(int(gl_InstanceID), in_tangent.xyz)), in_tangent.w), -1.0, 1.0);
    vec4 outpoint = (VPMatrix) * vec4(Output.WorldPos, 1);
//    outpoint.w = 0.5 + 0.5 * outpoint.w;
    //outpoint.w = - outpoint.w;
    MeshInstanceID = ModelInfos[int(gl_InstanceID)].idAnd4Empty.x;
    Output.Data.x = 1.0;
    Output.Data.y =  ( outpoint.z ) * 0.5 + 0.5 ;
    gl_Position = outpoint;// + vec4(0, 0.9, 0, 0);
}
