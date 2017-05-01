#version 430 core

#include PostProcessEffectBase.glsl

#define CLOUD_SAMPLES 2
#define CLOUDCOVERAGE_DENSITY 90
#include Atmosphere.glsl

uniform int RenderPass;

vec4 blurshadowsAOXA(samplerCube s, vec3 dir){
    float aoc = 0.0;

	vec4 cluma = vec4(0.0);
    float blurrange = 0.0001 * dir.y * dir.y;
    for(int i=0;i<17;i++){
        vec3 rdp = normalize(dir + randpoint3() * blurrange);
        cluma += textureLod(s, rdp, 0.0).rgba;
        aoc += 1.0;
    }
    cluma /= aoc;
    //return centerval;
    return clamp(cluma, 0.0, 1000000.0);
}

vec4 shade(){
   // return vec4(0);
    vec3 dir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    if(dir.y < -0.01) return vec4(0.0);
    vec4 retedg = vec4(0);
    vec4 retavg = vec4(0);

    if(RenderPass == 0){
    vec4 lastData = blurshadowsAOXA(coverageDistTex, dir);
        vec2 val = raymarchCloudsRay();
        //retedg.rg = vec2(max(val.r, lastData.r), min(val.g, lastData.g));
        retavg.rg = vec2(mix(val.r, lastData.r, CloudsIntegrate), val.g);
	//	retavg.r = mix(retavg.r, retedg.r, 0.2);
		//retavg.g = mix(retavg.g, retedg.g, 0.3);
    } else if(RenderPass == 1){
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
        vec4 last = blurshadowsAOXA(shadowsTex, dir);
        float shads = shadows();
        retavg.r = mix(shads, last.r, CloudsIntegrate);
        vec3 AOGround = getCloudsAL(dir, retavg.r);
        retavg.gba = mix(AOGround, last.gba, CloudsIntegrate);
    }

    return retavg;
}
