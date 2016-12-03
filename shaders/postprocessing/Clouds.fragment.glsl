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
		
		retavg.r = mix(retavg.r, retedg.r, 0.1);
		retavg.g = mix(retavg.g, retedg.g, 0.1);
    } else if(RenderPass == 1){
        vec4 lastData = texture(cloudsCloudsTex, dir).rgba;
        float val = shadows();
        retedg.r = min(val, lastData.r);
        retavg.r = mix(val, lastData.r, CloudsIntegrate);
		float AOGround = getCloudsAO(dir, 0.0);
		float AOSky = getCloudsAO(dir, 1.0);
        retedg.g = min(AOGround, lastData.g);
        retavg.g = mix(AOGround, lastData.g, CloudsIntegrate);
        retedg.b = min(AOSky, lastData.b);
        retavg.b = mix(AOSky, lastData.b, CloudsIntegrate);
		
		retavg.r = mix(retavg.r, retedg.r, 0.3);
		retavg.g = mix(retavg.g, retedg.g, 0.1);
		retavg.b = mix(retavg.b, retedg.b, 0.1);
    }

    return retavg;
}