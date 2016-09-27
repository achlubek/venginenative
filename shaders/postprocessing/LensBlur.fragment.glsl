#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 16) uniform sampler2D inputTex;

uniform float Time;

float rand2sTime(vec2 co){
    co *= Time;
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
/*
vec3 lensblur(vec2 uv){
    
}*/

vec4 shade(){    
    //vec3 color = fxaa(inputTex, UV).rgb;
    vec4 color = texture(inputTex, UV).rgba;
    return vec4(color);
}