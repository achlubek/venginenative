#version 430 core

in Data {
#include InOutStageLayout.glsl
} Input;

uniform uint MeshID;
uniform uint LodLevelID;
flat in uint MeshInstanceID;

layout(location = 0) out uvec4 IdOut;
layout(location = 1) out vec4 WorldPosOut;
layout(location = 2) out vec4 NormalOut;


void main(){
    IdOut = uvec4(MeshID, LodLevelID, MeshInstanceID, 0);
    WorldPosOut = vec4(Input.WorldPos, 0.0);
    NormalOut = vec4(normalize(Input.Normal), 0.0);
    //gl_FragDepth = clamp(Input.Data.y, 0.0, 1.0);
}
