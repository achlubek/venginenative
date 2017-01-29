#version 430 core

in Data {
#include InOutStageLayout.glsl
} Input;

uniform uint MeshID;
uniform uint LodLevelID;
flat in uint MeshInstanceID;

out uvec4 Red;


void main(){
    Red = uvec4(MeshID, LodLevelID, MeshInstanceID, 0);
    //gl_FragDepth = clamp(Input.Data.y, 0.0, 1.0);
}
