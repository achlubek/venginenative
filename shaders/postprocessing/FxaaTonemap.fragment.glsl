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
        vec3 pix = vec3(1.0 / Resolution.x, 1.0 / Resolution.y, 0.0);
        float d1 = 0.0;//currentData.cameraDistance;
        d1 += textureLod(mrt_Distance_Bump_Tex, UV + pix.xz * 2.0, 1.0).r;
        d1 += textureLod(mrt_Distance_Bump_Tex, UV + pix.zy * 2.0, 1.0).r;
        d1 += textureLod(mrt_Distance_Bump_Tex, UV - pix.xz * 2.0, 1.0).r;
        d1 += textureLod(mrt_Distance_Bump_Tex, UV - pix.zy * 2.0, 1.0).r;
        d1 /= 4.0;
        float distance_mult = smoothstep(0.01, 0.01, abs(currentData.cameraDistance - d1));
        vec3 n1 = vec3(0.0);//currentData.cameraDistance;
        n1 += textureLod(mrt_Normal_Metalness_Tex, UV + pix.xz * 2.0, 1.0).rgb;
        n1 += textureLod(mrt_Normal_Metalness_Tex, UV + pix.zy * 2.0, 1.0).rgb;
        n1 += textureLod(mrt_Normal_Metalness_Tex, UV - pix.xz * 2.0, 1.0).rgb;
        n1 += textureLod(mrt_Normal_Metalness_Tex, UV - pix.zy * 2.0, 1.0).rgb;
        n1 /= 4.0;
        float normal_mult = smoothstep(0.0, 0.1, 1.0 - max(0.0, dot(currentData.normal, n1)));
        //return vec4(smoothstep(0.01, 0.1, abs(currentData.cameraDistance - d1)));
        vec4 blurred = textureLod(inputTex, UV, 1.0);
        blurred += textureLod(inputTex, UV + pix.xz * 2.0, 1.0);
        blurred += textureLod(inputTex, UV - pix.xz * 2.0, 1.0);
        blurred += textureLod(inputTex, UV + pix.zy * 2.0, 1.0);
        blurred += textureLod(inputTex, UV - pix.zy * 2.0, 1.0);
        blurred /= 5.0;
    //    return blurred;
        return  mix(textureLod(inputTex, UV, 0.0).rgba, fxaa(inputTex, UV).rgba, max(distance_mult, normal_mult));
    } else {
        vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
        float lastinter = textureLod(temporalBBTex, UV, 0.0).a;
        vec4 lastpos = textureLod(temporalWposBBTex, UV, 0.0).rgba;
        vec2 olduv = projectvdao(lastpos.xyz);
        vec2 nuv = UV + (UV - olduv);
        float iter = rand2sTime(UV) * 0.1;
        vec4 last = textureLod(temporalBBTex, nuv, 0.0).rgba;

        vec3 color =  textureLod(inputTex, UV, 0.0).rgb;
        for(int i=0;i<10;i++){

            color += textureLod(inputTex, clamp(mix(UV, nuv, iter), 0.0, 1.0), 0.0).rgb;
            iter += 0.1;
        }
        color *= 0.1;

        float distance_fix = 1.0 - smoothstep(0.001, 0.003, distance(lastpos.xyz, currentData.worldPos));
        distance_fix *= 1.0 - smoothstep(0.001, 0.003, abs(lastpos.a - distance(lastpos.xyz, CameraPosition)));
        float procedural_fix = step(0.01, currentData.cameraDistance);
        float out_of_screen_fix = (nuv.x < 0.0 || nuv.x > 1.0 || nuv.y < 0.0 || nuv.y > 1.0) ? 0.0 : 1.0;


        color = mix(color, last.rgb, distance_fix * procedural_fix * out_of_screen_fix * lastinter);// (nuv.x < 0.0 || nuv.x > 1.0 || nuv.y < 0.0 || nuv.y > 1.0) ? 0.0 : ( step(0.01, currentData.cameraDistance)) * (0.95 / (1.0 + 1110.0 * abs(currentData.cameraDistance - last.a))));
        outWpos = vec4(currentData.worldPos, distance(currentData.worldPos, CameraPosition));

        if(currentData.cameraDistance < 0.01) outWpos.xyz = CameraPosition + dir * rsi2(Ray(toplanetspace(CameraPosition), dir), Sphere(vec3(0.0), planetradius + atmosphereradius));
        return vec4(color, mix(distance_fix * procedural_fix * out_of_screen_fix * 0.98, lastinter, 0.8));
    }
}
