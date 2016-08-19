#version 430 core

#include PostProcessEffectBase.glsl

#define CLOUD_SAMPLES 2
#define CLOUDCOVERAGE_DENSITY 90
#include Atmosphere.glsl

vec4 shade(){    
   // if(shouldBreak()) return vec4(0);
    vec3 dir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    
    vec4 lastData = texture(cloudsCloudsTex, dir).rgba;
   vec4 val = raymarchCloudsRay();
   /*
   //val.g = min(val.g, lastData.a);
    //return mix(lastData, vec4(mix(vec3(val.g), scatt, val.r), 1.0), 0.3);
    vec4 bufferedRes = val.rggg;
    bufferedRes.r = max(lastData.r, val.r);
    bufferedRes.g = mix(val.g, min(lastData.g, val.g), 0.99);
    //return vec4(0,1,0,0);
    bufferedRes = mix(val, bufferedRes, 0.99);
    bufferedRes = mix(lastData, bufferedRes, 0.6);
    return bufferedRes;*/
  //  vec4 data = val.b <= lastData.b ? val : lastData;
  //  vec3 pos = startpos + ssdir * data.b;
   // data.g = getAOPos(1.0, pos);
   val.g = mix(min(val.g, lastData.g), val.g, 0.3);
  // val.a = mix(min(val.a, lastData.a), val.a, 0.3);
   val.r = mix(max(val.r, lastData.r), val.r, 0.2);
   float s = val.b;
    val = mix(val, lastData, 0.43981818227);
    val.b = s;
   // data = mix(val, data, 0.95);
   
    return val;
  //  return vec4(0,1,999999999,0);
}