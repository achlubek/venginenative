#version 430 core

#include PostProcessEffectBase.glsl
#include Constants.glsl
#include PlanetDefinition.glsl

layout(location = 1) out vec4 outWpos;

layout(binding = 16) uniform sampler2D inputTex;
layout(binding = 15) uniform sampler2D temporalBBTex;
layout(binding = 14) uniform sampler2D temporalWposBBTex;

#include FXAA.glsl

uniform float Time;
uniform int IsFinalPass;
vec2 projectvdao(vec3 pos){
    vec4 tmp = (VPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}
float rand2sTime(vec2 co){
    co *= Time;
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
vec4 shade(){
    if(IsFinalPass == 1){
        return fxaa(inputTex, UV).rgba;
    } else {
        vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
        float lastinter = textureLod(temporalBBTex, UV, 0.0).a;
        vec4 lastpos = textureLod(temporalWposBBTex, UV, 0.0).rgba;
        vec2 olduv = projectvdao(lastpos.xyz);
        vec2 nuv = UV + (UV - olduv);
        float iter = rand2sTime(UV) * 0.1;
        vec4 last = textureLod(temporalBBTex, nuv, 0.0).rgba;

        vec3 color = textureLod(inputTex, UV, 0.0).rgb;
        /*for(int i=0;i<10;i++){

            color += textureLod(inputTex, mix(UV, nuv, iter), 0.0).rgb;
            iter += 0.1;
        }
        color *= 0.1;*/

        float distance_fix = 1.0 - smoothstep(0.001, 0.003, distance(lastpos.xyz, currentData.worldPos));
        distance_fix *= 1.0 - smoothstep(0.001, 0.003, abs(lastpos.a - distance(lastpos.xyz, CameraPosition)));
        float procedural_fix = step(0.01, currentData.cameraDistance);
        float out_of_screen_fix = (nuv.x < 0.0 || nuv.x > 1.0 || nuv.y < 0.0 || nuv.y > 1.0) ? 0.0 : 1.0;


        color = mix(color, last.rgb, distance_fix * procedural_fix * out_of_screen_fix * lastinter);// (nuv.x < 0.0 || nuv.x > 1.0 || nuv.y < 0.0 || nuv.y > 1.0) ? 0.0 : ( step(0.01, currentData.cameraDistance)) * (0.95 / (1.0 + 1110.0 * abs(currentData.cameraDistance - last.a))));
        outWpos = vec4(currentData.worldPos, distance(currentData.worldPos, CameraPosition));
        return vec4(color, mix(distance_fix * procedural_fix * out_of_screen_fix * 0.98, lastinter, 0.8));
    }
}
