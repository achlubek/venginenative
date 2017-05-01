#version 430 core

layout(binding = 14) uniform sampler2D inputTex;

in vec2 UV;
layout(location = 0) out vec4 outcolor;

void main(){
    outcolor = texture(inputTex, UV);
}
