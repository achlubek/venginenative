#version 430 core

#include PostProcessEffectBase.glsl

#define CLOUD_SAMPLES 2
#define CLOUDCOVERAGE_DENSITY 90
#include Atmosphere.glsl

uniform int RenderPass;

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
		retavg.g = mix(retavg.g, retedg.g, 0.5);
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
		retavg.b = mix(retavg.b, retedg.b, 0.5);*/
		retavg.r = mix(retavg.r, retedg.r, 0.5);
		
        retavg.gba = mix(AOGround, lastData.gba, CloudsIntegrate);
    }

    return retavg;
}