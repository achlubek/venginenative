#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 16) uniform sampler2D inputTex;

#include FXAA.glsl

uniform float Time;

vec4 shade(){
    vec3 color = fxaa(inputTex, UV).rgb;
   // vec3 t = textureLod(inputTex, UV, 1.0).rgb;
   // float d = distance(color, t);
  //  color = mix(color, textureLod(inputTex, UV, 2.0).rgb, smoothstep(0.0,  length(vec3(1.0)),d));

   // vec3 color = textureLod(inputTex, UV, float(textureQueryLevels(inputTex) - 2.0)).rgb;
   // vec3 color = BoKeH(UV);
   //return vec4(vignette(UV, 1.2, 1.0));
    return vec4((color), 1.0);
}
