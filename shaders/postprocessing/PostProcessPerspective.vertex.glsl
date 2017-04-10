#version 430 core
#include PostProcessingCommonUniforms.glsl
layout(location = 0) in vec3 in_position;

void main(){
    gl_Position =  VPMatrix * (LightMMatrix * vec4(in_position.xyz,1));
}
