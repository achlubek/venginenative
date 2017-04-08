#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 16) uniform sampler2D inputTex;
layout(binding = 15) uniform sampler2D temporalBBTex;

#include FXAA.glsl

uniform float Time;
uniform int IsFinalPass;
vec4 shade(){
    if(IsFinalPass == 1){
        return textureLod(inputTex, UV, 0.0).rgba;
    } else {
        vec3 color = fxaa(inputTex, UV).rgb;
        vec4 last = textureLod(temporalBBTex, UV, 0.0).rgba;
        color = mix(color, last.rgb, 0.97);// / (1.0 + 10.0 * abs(currentData.cameraDistance - last.a)));
        return vec4((color), currentData.cameraDistance);
    }
}
