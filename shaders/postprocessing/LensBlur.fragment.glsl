#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 16) uniform sampler2D inputTex;

uniform float Time;

vec4 shade(){
    //vec3 color = fxaa(inputTex, UV).rgb;
    return vec4(1.0);
}
