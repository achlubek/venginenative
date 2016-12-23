#version 430 core

#include PostProcessEffectBase.glsl
#include Shade.glsl

layout(binding = 10) uniform samplerCube probeTex;
layout(binding = 16) uniform sampler2D aoxTex;
layout(binding = 3) uniform samplerCube skyboxTex;

float rand2s(vec2 co){
        return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

#define MMAL_LOD_REGULATOR 1024
vec3 stupidBRDF(vec3 dir, float level, float roughness){
    return textureLod(probeTex, dir , level).rgb;
}

vec3 MMALSkybox(vec3 dir, float roughness){
    //roughness = roughness * roughness;
    float levels = max(0, float(textureQueryLevels(probeTex)) - 2.0);
    float mx = log2(roughness*MMAL_LOD_REGULATOR+1)/log2(MMAL_LOD_REGULATOR);
    vec3 result = stupidBRDF(dir, mx * levels, roughness);
    
    //return pow(result * 1.2, vec3(2.0));
    return result;
}

uniform vec3 EnvProbePosition;
uniform int EnvProbePlanesCount;
#define MAX_ENV_PROBE_PLANES 20
uniform vec3 EnvProbePlanesPoints[MAX_ENV_PROBE_PLANES];
uniform vec3 EnvProbePlanesNormals[MAX_ENV_PROBE_PLANES];
uniform float EnvProbesLightMultiplier;

float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{ return dot(point - origin, normal) / dot(direction, normal); }

float currentFalloff = 1.0;
vec3 ENVMMALMetallic(PostProcessingData data, vec3 dir){
    
    float fresnelR = fresnel_again(vec3(data.diffuseColor.r), data.normal, data.cameraPos, data.roughness);
    float fresnelG = fresnel_again(vec3(data.diffuseColor.g), data.normal, data.cameraPos, data.roughness);
    float fresnelB = fresnel_again(vec3(data.diffuseColor.b), data.normal, data.cameraPos, data.roughness);
    vec3 newBase = vec3(fresnelR, fresnelG, fresnelB);
    
    vec3 metallic = vec3(0);
    
    metallic += MMALSkybox(dir, data.roughness) * newBase;
    
    return metallic;
    
}

vec3 ENVMMALNonMetallic(PostProcessingData data, vec3 dir){
    
    float fresnel = fresnel_again(vec3(0.04), data.normal, data.cameraPos, data.roughness);
    
    vec3 nonmetallic = vec3(0);
    
    nonmetallic += MMALSkybox(dir, data.roughness) * fresnel;
    nonmetallic += MMALSkybox(dir, 1.0) *  data.diffuseColor * (1.0 - fresnel);
    
    return nonmetallic;
    
}
vec3 ENVMMALNonMetallicDiffuse(PostProcessingData data, vec3 dir){
    
    vec3 nonmetallic = vec3(0);
    
    nonmetallic += MMALSkybox(dir, 1.0) *  data.diffuseColor;
    
    return nonmetallic;
    
}
vec3 raytracePlanes(vec3 origin, vec3 direction){
    float mindist = 999999.0;
    for(int i=0;i<EnvProbePlanesCount;i++){
        float inter = intersectPlane(origin, direction, EnvProbePlanesPoints[i], EnvProbePlanesNormals[i]);
        if(dot(direction, -EnvProbePlanesNormals[i]) > 0) mindist = min(mindist, inter);
    }
    vec3 c = vec3(0);
    if(mindist < 999990.0){
        vec3 newpos = (origin + direction * mindist);
        vec3 newdir = -normalize(EnvProbePosition - newpos);
        //float att = CalculateFallof(0.05 * abs(dst - mindist));
      //  currentData.roughness = currentData.roughness * (distance(newpos, currentData.worldPos));
        //currentFalloff = att;//CalculateFallof(0.01*distance(newpos, currentData.worldPos));
        c += ENVMMALMetallic(currentData, newdir);
        c += ENVMMALNonMetallicDiffuse(currentData, newdir);
    } else {
        c = ENVMMALMetallic(currentData, direction);
    }
    return c * EnvProbesLightMultiplier;
}

vec3 rayMarchDepthSpeculars(vec3 origin, vec3 direction){
    vec3 closest = vec3(0,1,0);
    vec3 meter = origin;
    float vis = 18.0;
    vec3 colo = vec3(0);
    float weight = 0.0;
    float weight2 = 1.0;
    for(int i=0;i<25;i++){
        float inter = textureLod(probeTex, normalize(meter - EnvProbePosition), 0).a;
        float dst = 1.0 - smoothstep(0.0, 1.0, abs(inter - distance(meter, EnvProbePosition)));
        
        closest = normalize(meter - EnvProbePosition); 
        weight += dst;
        vec3 met = ENVMMALMetallic(currentData, closest) * dst;
        vec3 nonmet = ENVMMALMetallic(currentData, closest) * dst;
        colo += mix(nonmet, met, currentData.metalness);
        
        meter = origin + direction * vis;
        vis *= 0.8;
    }
    colo /= max(0.01, weight);

    return clamp(colo, 0.0, 1.0) * EnvProbesLightMultiplier;
}
vec3 rayMarchDepthDiffuse(vec3 origin, vec3 direction){
    vec3 closest = vec3(0,1,0);
    vec3 meter = origin;
    float vis = 18.0;
    vec3 colo = vec3(0);
    float weight = 0.0;
    float weight2 = 1.0;
    for(int i=0;i<5;i++){
        float dst2 = CalculateFallof(distance(origin, meter));
        
        closest = normalize(meter - EnvProbePosition); 
        weight += dst2;
        colo += ENVMMALNonMetallicDiffuse(currentData, closest) * dst2;
        
        meter = origin + direction * vis;
        vis *= 0.9;
    }
    colo /= max(0.01, weight);

    return clamp(colo, 0.0, 1.0) * EnvProbesLightMultiplier;
}

    
uniform float Time;

vec4 shade(){
    vec4 color = vec4(0);
    if(currentData.cameraDistance > 0){
        vec3 reflected = normalize(reflect(currentData.cameraPos, currentData.normal));
        vec3 dir = normalize(mix(reflected, currentData.normal, currentData.roughness));
        float ao = EnvProbesLightMultiplier == 1.0 ? texture(aoxTex, UV).r : 1.0;
        ao = mix(1.0, ao, currentData.roughness);
      /*  color.rgb += mix(1.0, ao, currentData.roughness) * rayMarchDepthSpeculars(currentData.worldPos, dir);
        color.rgb += ao * rayMarchDepthDiffuse(currentData.worldPos, currentData.normal);*/
       // color.rgb += pow(ao, 3 * (1.0 - currentData.roughness) + 1.0) * raytracePlanes(currentData.worldPos, dir);
    }
   // color.rgb += (1.0 - smoothstep(0.0, 0.001, textureLod(mrt_Distance_Bump_Tex, UV, 0).r)) * pow(textureLod(skyboxTex, reconstructCameraSpaceDistance(UV, 1.0), 0.0).rgb, vec3(2.4));
    return clamp(color.rgbb, 0.0, 11.0) * 0.25;
}