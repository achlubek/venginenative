#version 450 core

layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;

uniform vec2 uv;
layout(binding = 0)  uniform usampler2D dataTex;
layout(binding = 1)  uniform sampler2D wpTex;
layout(binding = 2)  uniform sampler2D nTex;

layout (std430, binding = 5) coherent buffer OutBuffer
{
    uvec4 IdOut;
    vec4 WorldPosOut;
    vec4 NormalOut;
};

void main(){
    IdOut = textureLod(dataTex, uv, 0.0);
    WorldPosOut = textureLod(wpTex, uv, 0.0);
    NormalOut = textureLod(nTex, uv, 0.0);
}
