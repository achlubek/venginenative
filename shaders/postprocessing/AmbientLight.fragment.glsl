#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 3) uniform samplerCube skyboxTex;
layout(binding = 24) uniform sampler2DArray CSMTex;
#include Atmosphere.glsl

#define MAX_CSM_LAYERS 9
uniform int CSMLayers;
uniform mat4 CSMPMatrices[MAX_CSM_LAYERS];
uniform mat4 CSMVPMatrices[MAX_CSM_LAYERS];
uniform float CSMFarplanes[MAX_CSM_LAYERS];
uniform float CSMRadiuses[MAX_CSM_LAYERS];
uniform vec3 CSMCenter;
uniform mat4 CSMVMatrix;

vec2 CSMProject(vec3 pos, int layer, out float depth){
    vec4 tmp = ((CSMVPMatrices[layer]) * vec4(pos, 1.0));
    depth = ((tmp.z / tmp.w) * 0.5 + 0.5);
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}

float CSMQueryVisibility(vec3 pos){
    for(int i=0;i<CSMLayers;i++){
        float depth = 0.0;
        vec2 csmuv = CSMProject(pos, i, depth);
        if(csmuv.x >= 0.0 && csmuv.x < 1.0 && csmuv.y >= 0.0 && csmuv.y < 1.0){
            return (texture(CSMTex, vec3(csmuv, i)).r - depth - 0.001);
        }
    }
    return 1.0;
}

float rand2sx(vec2 co){
        return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

#define MMAL_LOD_REGULATOR 1024
vec3 stupidBRDF(vec3 dir, float level, float roughness){
    vec3 aaprc = vec3(0.0);
    float xx=rand2s(UV);
    float xx2=rand2s(UV.yx);
    for(int x = 0; x < 14; x++){
        vec3 rd = vec3(
            rand2s(vec2(xx, xx2)),
            rand2s(vec2(-xx2, xx)),
            rand2s(vec2(xx2, xx))
        ) *2-1;
        vec3 displace = rd;
        vec3 prc = textureLod(skyboxTex, dir + (displace * 0.1 * roughness), level).rgb * 5.0;
        aaprc += prc;
        xx += 0.01;
        xx2 -= 0.02123;
    }
    return pow(aaprc / 14.0, vec3(2.4));
}

vec3 MMALSkybox(vec3 dir, float roughness){
    //roughness = roughness * roughness;
    float levels = max(0, float(textureQueryLevels(skyboxTex)) - 1);
    float mx = log2(roughness*MMAL_LOD_REGULATOR+1)/log2(MMAL_LOD_REGULATOR);
    vec3 result = pow(textureLod(skyboxTex, dir, mx * levels).rgb, vec3(2.4)) * 5;
    
    //return pow(result * 1.2, vec3(2.0));
    return result;
}

vec3 atmc = getAtmosphereForDirection(currentData.worldPos, currentData.normal, normalize(SunDirection), currentData.roughness);

vec3 shadingMetalic(PostProceessingData data){
    float fresnelR = fresnel_again(vec3(data.diffuseColor.r), data.normal, data.cameraPos, data.roughness);
    float fresnelG = fresnel_again(vec3(data.diffuseColor.g), data.normal, data.cameraPos, data.roughness);
    float fresnelB = fresnel_again(vec3(data.diffuseColor.b), data.normal, data.cameraPos, data.roughness);
    float fresnel = fresnel_again(vec3(0.04), data.normal, normalize(data.cameraPos), data.roughness);
    vec3 newBase = vec3(fresnelR, fresnelG, fresnelB);
 //   return vec3(fresnel);
    float x = 1.0 - max(0, -dot(normalize(SunDirection), currentData.originalNormal));
    return shade(CameraPosition, newBase, data.normal, data.worldPos, data.worldPos + normalize(SunDirection) * 40.0, vec3(atmc),  max(0.004, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - currentData.roughness);
}

vec3 shadingNonMetalic(PostProceessingData data){
    float fresnel = fresnel_again(vec3(data.diffuseColor.g), data.normal, normalize(data.cameraPos), data.roughness);
    float x = 1.0 - max(0, -dot(normalize(SunDirection), currentData.originalNormal));
    
    vec3 radiance = shade(CameraPosition, vec3(fresnel), data.normal, data.worldPos, data.worldPos + normalize(SunDirection) * 40.0, vec3(atmc), max(0.004, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - currentData.roughness);    
    
    vec3 difradiance = shadeDiffuse(CameraPosition, data.diffuseColor * (1.0 - fresnel), data.normal, data.worldPos, data.worldPos + normalize(SunDirection) * 40.0, vec3(atmc), 0.0, false) * x;
 //   return vec3(0);
    return radiance + difradiance ;
}

vec3 MakeShading(PostProceessingData data){
    return mix(shadingNonMetalic(data), shadingMetalic(data), data.metalness);
}
vec3 MMAL(PostProceessingData data){
    vec3 reflected = normalize(reflect(data.cameraPos, data.normal));
    vec3 dir = normalize(mix(reflected, data.normal, data.roughness));
    
    float fresnel = fresnel_again(vec3(0.04), data.normal, data.cameraPos, data.roughness);
    float fresnelR = fresnel_again(vec3(data.diffuseColor.r), data.normal, data.cameraPos, data.roughness);
    float fresnelG = fresnel_again(vec3(data.diffuseColor.g), data.normal, data.cameraPos, data.roughness);
    float fresnelB = fresnel_again(vec3(data.diffuseColor.b), data.normal, data.cameraPos, data.roughness);
    vec3 newBase = vec3(fresnelR, fresnelG, fresnelB);
    
    vec3 metallic = vec3(0);
    vec3 nonmetallic = vec3(0);
    
    metallic += MMALSkybox(dir, data.roughness) * newBase;
    
    nonmetallic += MMALSkybox(dir, data.roughness) * fresnel;
    nonmetallic += MMALSkybox(dir, 1.0) *  data.diffuseColor * (1.0 - fresnel);
    
    return  MakeShading(currentData);
    
}


vec4 shade(){
    vec4 color = vec4(0);
    if(currentData.cameraDistance > 0){
        //color.rgb += MMAL(currentData) *0.63;
        atmc = mix(vec3(1.0), getAtmosphereForDirection(currentData.worldPos, normalize(SunDirection), normalize(SunDirection), 0.0), 1.0 - normalize(SunDirection).y);
        color.rgb += getAtmosphereForDirection(currentData.worldPos, currentData.normal, normalize(SunDirection), currentData.roughness) * 0.0 * currentData.diffuseColor + MakeShading(currentData) * CSMQueryVisibility(currentData.worldPos);
    }
    //color.rgb += (1.0 - smoothstep(0.0, 0.001, textureLod(mrt_Distance_Bump_Tex, UV, 0).r)) * pow(textureLod(skyboxTex, reconstructCameraSpaceDistance(UV, 1.0), 0.0).rgb, vec3(2.4)) * 5.0;
    return clamp(color, 0.0, 1.0);
}