#version 430 core

#include PostProcessEffectBase.glsl

#define CLOUD_SAMPLES 2
#define CLOUDCOVERAGE_DENSITY 90
#include Atmosphere.glsl

uniform int RenderPass;

vec4 shade(){    
   // if(shouldBreak()) return vec4(0);
    vec3 dir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    vec4 ret = vec4(0);
    if(RenderPass == 0){
        vec2 lastData = texture(cloudsCloudsTex, dir).rg;
        vec2 val = raymarchCloudsRay();
        val = mix(val, lastData, CloudsIntegrate);
        ret.rg = val;
    } else if(RenderPass == 1){
        float lastData = texture(cloudsCloudsTex, dir).r;
        float val = shadows();
        val = mix(val, lastData, CloudsIntegrate);
        ret.r = val;
    } else {
        float lastData = texture(cloudsCloudsTex, dir).r;
        float val = skyfog();
        val = mix(val, lastData, CloudsIntegrate);
        ret.r = val;
    }
    return ret;
}