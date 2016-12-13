#version 430 core

#include PostProcessEffectBase.glsl

#define CLOUD_SAMPLES 2
#define CLOUDCOVERAGE_DENSITY 90
#include Atmosphere.glsl

uniform int RenderPass;

float rdhash = 0.453451 + Time;
vec3 randpoint3(){
    float x = rand2s(UV * rdhash);
    rdhash += 2.1231255;
    float y = rand2s(UV * rdhash);
    rdhash += 1.6271255;
    float z = rand2s(UV * rdhash);
    rdhash += 1.1231255;
    return vec3(x, y, z) * 2.0 - 1.0;
}
vec3 blurshadowsAOXA(vec3 dir, float roughness){
    //if(CloudsDensityScale <= 0.010) return 0.0;
    float levels = max(0, float(textureQueryLevels(cloudsCloudsTex)));
    float mx = log2(roughness*1024+1)/log2(1024);
    float mlvel = mx * levels;
//	return textureLod(cloudsCloudsTex, dir, mlvel).gba;
   // float dst = textureLod(coverageDistTex, dir, mlvel).g;
    float aoc = 1.0;
    
    vec3 centerval = textureLod(cloudsCloudsTex, dir, mlvel).gba;
	float cluma = length(centerval);
    float blurrange = 0.0003;
    for(int i=0;i<7;i++){
        vec3 rdp = normalize(dir + randpoint3() * blurrange);
        //float there = textureLod(coverageDistTex, rdp, mlvel).g;
        //float w = clamp(1.0 / (abs(there - dst)*0.01 + 0.01), 0.0, 1.0);
        vec3 th = textureLod(cloudsCloudsTex, rdp, mlvel).gba;
		
        cluma += length(textureLod(cloudsCloudsTex, rdp, mlvel).gba);
        aoc += 1.0;
    }
    cluma /= aoc;
    //return centerval;
    return normalize(centerval) * cluma;
}
vec4 shade(){    
   // return vec4(0);
    vec3 dir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    vec4 retedg = vec4(0);
    vec4 retavg = vec4(0);
    
    if(RenderPass == 0){
        vec2 lastData = texture(cloudsCloudsTex, dir).rg;
        vec2 val = raymarchCloudsRay();
        retedg.rg = vec2(max(val.r, lastData.r), min(val.g, lastData.g));
        retavg.rg = vec2(mix(val.r, lastData.r, CloudsIntegrate), val.g);
		
		retavg.r = mix(retavg.r, retedg.r, 0.2);
		retavg.g = mix(retavg.g, retedg.g, 0.3);
    } else if(RenderPass == 1){
        vec4 lastData = textureLod(cloudsCloudsTex, dir, 0.0).rgba;
        float val = shadows();
		vec3 AOGround = getCloudsAL(dir);
		//float AOSky = 1.0 - AOGround;//getCloudsAO(dir, 1.0);
				
        retedg.r = min(val, lastData.r);
        retavg.r = mix(val, lastData.r, CloudsIntegrate);
		/*
        retedg.g = min(AOGround, lastData.g);
        retavg.g = mix(AOGround, lastData.g, CloudsIntegrate);
        retedg.b = min(AOSky, lastData.b);
        retavg.b = mix(AOSky, lastData.b, CloudsIntegrate);
		
		retavg.g = mix(retavg.g, retedg.g, 0.5);
		retavg.b = mix(retavg.b, retedg.b, 0.5);
		retavg.r = mix(retavg.r, retedg.r, 0.1);*/
		retavg.r = mix(retavg.r, retedg.r, 0.2);
		
        retavg.gba = mix(AOGround, blurshadowsAOXA(dir, 0.0), CloudsIntegrate);
    }

    return retavg;
}