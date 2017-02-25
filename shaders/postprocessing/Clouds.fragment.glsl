#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 3) uniform samplerCube skyboxTex;
layout(binding = 5) uniform sampler2D directTex;
layout(binding = 6) uniform sampler2D alTex;
layout(binding = 16) uniform sampler2D aoxTex;
layout(binding = 20) uniform sampler2D fogTex;
layout(binding = 21) uniform sampler2D waterColorTex;
layout(binding = 22) uniform sampler2D inputTex;

layout(binding = 25) uniform samplerCube coverageDistTex;
layout(binding = 26) uniform samplerCube shadowsTex;
layout(binding = 27) uniform samplerCube skyfogTex;
layout(binding = 28) uniform sampler2D moonTex;
layout(binding = 23) uniform sampler2D waterTileTex;
layout(binding = 24) uniform sampler2D starsTex;
layout(binding = 29) uniform samplerCube resolvedAtmosphereTex;
#define CLOUD_SAMPLES 2
#define CLOUDCOVERAGE_DENSITY 90
#include Atmosphere.glsl

uniform int RenderPass;

vec4 blurshadowsAOXA(vec3 dir, float roughness){
    //if(CloudsDensityScale <= 0.010) return 0.0;
    float levels = max(0, float(textureQueryLevels(cloudsCloudsTex)));
    float mx = log2(roughness*1024+1)/log2(1024);
    float mlvel = mx * levels;
	return textureLod(cloudsCloudsTex, dir, mlvel).rgba;
   // float dst = textureLod(coverageDistTex, dir, mlvel).g;
    float aoc = 1.0;

    vec4 centerval = textureLod(cloudsCloudsTex, dir, mlvel).rgba;
	float cluma = length(centerval);
    float blurrange = 0.002 * dir.y;
    for(int i=0;i<17;i++){
        vec3 rdp = normalize(dir + randpoint3() * blurrange);
        //float there = textureLod(coverageDistTex, rdp, mlvel).g;
        //float w = clamp(1.0 / (abs(there - dst)*0.01 + 0.01), 0.0, 1.0);
        vec4 th = textureLod(cloudsCloudsTex, rdp, mlvel).rgba;

        cluma += length(textureLod(cloudsCloudsTex, rdp, mlvel).rgba);
        aoc += 1.0;
    }
    cluma /= aoc;
    //return centerval;
    return normalize(centerval) * cluma;
}

vec4 shade(){
   // return vec4(0);
    vec3 dir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    if(dir.y < -0.01) return vec4(0.0);
    vec4 retedg = vec4(0);
    vec4 retavg = vec4(0);

    if(RenderPass == 0){
        vec2 lastData = texture(cloudsCloudsTex, dir).rg;
        vec2 val = raymarchCloudsRay();
        retedg.rg = vec2(max(val.r, lastData.r), min(val.g, lastData.g));
        retavg.rg = vec2(mix(val.r, lastData.r, CloudsIntegrate), mix(val.g, lastData.g, CloudsIntegrate * 0.8));
	//	retavg.r = mix(retavg.r, retedg.r, 0.2);
	//	retavg.g = mix(retavg.g, retedg.g, 0.3);
    } else if(RenderPass == 1){
        vec4 lastData = textureLod(cloudsCloudsTex, dir, 0.0).rgba;
		vec3 AOGround = getCloudsAL(dir);
    //    AOGround += (SunC * val * SunDT);
		/*
        retedg.g = min(AOGround, lastData.g);
        retavg.g = mix(AOGround, lastData.g, CloudsIntegrate);
        retedg.b = min(AOSky, lastData.b);
        retavg.b = mix(AOSky, lastData.b, CloudsIntegrate);

		retavg.g = mix(retavg.g, retedg.g, 0.5);
		retavg.b = mix(retavg.b, retedg.b, 0.5);
		retavg.r = mix(retavg.r, retedg.r, 0.1);*/
    //    vec4 brl = blurshadowsAOXA(dir, 0.0);
    //    retedg.r = min(val, brl.r);
    //    retavg.r = mix(val, brl.r, CloudsIntegrate);
	//	retavg.r = mix(retavg.r, retedg.r, 0.2);

        //vec3 blr = blurshadowsAOXA(dir, 0.0);
        vec4 last = textureLod(cloudsCloudsTex, dir, 0.0).rgba;
        retavg.r = mix(globalAoOutput, last.r, CloudsIntegrate);
        retavg.gba = mix(AOGround, last.gba, CloudsIntegrate);
    }

    return retavg;
}
