#version 430 core
#define RECREATE_UV
#include PostProcessEffectBase.glsl

layout(binding = 14) uniform sampler2DShadow shadowMapSingle;
layout(binding = 15) uniform samplerCube shadowMapCube;
layout(binding = 16) uniform sampler2D aoxTex;

uniform int UseAO;

float lookupAO(vec2 fuv, float radius){
        
    if(UseAO == 0) {
        return 1.0;
    } else {
        return  textureLod(aoxTex, fuv, 0).r ;
        /*
        float ratio = Resolution.y/Resolution.x;
        float outc = 0;
        float counter = 0;
        float depthCenter = currentData.cameraDistance;
        vec3 normalcenter = currentData.normal;
        for(float g = 0; g < 6.283; g+=0.9)
        {
            for(float g2 = 0; g2 < 1.0; g2+=0.33)
            {
                vec2 gauss = clamp(fuv + vec2(sin(g + g2*6)*ratio, cos(g + g2*6)) * (g2 * 0.012 * radius), 0.0, 1.0);
                float color = textureLod(aoxTex, gauss, 0).r;
                float depthThere = texture(mrt_Distance_Bump_Tex, gauss).a;
                vec3 normalthere = texture(mrt_Normal_Metalness_Tex, gauss).rgb;
                float weight = pow(max(0, dot(normalthere, normalcenter)), 32);
                outc += color * weight;
                counter+=weight;
            }
        }
        return  textureLod(aoxTex, fuv, 0).r ;*/
    }
}

#define LIGHT_SPOT 0
#define LIGHT_POINT 1
#define LIGHT_AMBIENT 2

uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform vec4 LightOrientation;
uniform float LightAngle;
uniform int LightType;
uniform int LightUseShadowMap;
uniform mat4 LightVPMatrix;
uniform float LightCutOffDistance;

#include Shade.glsl
#include Quaternions.glsl

#define KERNEL 6
#define PCFEDGE 1
float PCFDeferred(vec2 uvi, float comparison){

    float shadow = 0.0;
    float pixSize = 1.0 / textureSize(shadowMapSingle,0).x;
    float bound = KERNEL * 0.5 - 0.5;
    bound *= PCFEDGE;
    for (float y = -bound; y <= bound; y += PCFEDGE){
        for (float x = -bound; x <= bound; x += PCFEDGE){
            vec3 uv = vec3(uvi+ vec2(x,y)* pixSize, comparison + 0.001);
            shadow += texture(shadowMapSingle, uv);
        }
    }
    return 1.0 - shadow / (KERNEL * KERNEL);
}

float toLogDepth(float depth, float far){
    float badass_depth = log2(max(1e-6, 1.0 + depth)) / (log2(far));
    return badass_depth;
}

#define MIN_ROUGHNESS_DIRECT 0.04

vec3 shadingMetalic(PostProcessingData data){
    float fresnelR = fresnel_again(vec3(data.diffuseColor.r), data.normal, data.cameraPos, data.roughness);
    float fresnelG = fresnel_again(vec3(data.diffuseColor.g), data.normal, data.cameraPos, data.roughness);
    float fresnelB = fresnel_again(vec3(data.diffuseColor.b), data.normal, data.cameraPos, data.roughness);
    float fresnel = fresnel_again(vec3(0.04), data.normal, normalize(data.cameraPos), data.roughness);
    vec3 newBase = vec3(fresnelR, fresnelG, fresnelB);
 //   return vec3(fresnel);
    float x = 1.0 - max(0, -dot(normalize(LightPosition), currentData.originalNormal));
    return shade(CameraPosition, newBase, data.normal, data.worldPos, LightPosition, abs(LightColor),  max(MIN_ROUGHNESS_DIRECT, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - currentData.roughness);
}

vec3 shadingNonMetalic(PostProcessingData data){
    float fresnel = fresnel_again(vec3(0.04), data.normal, normalize(data.cameraPos), data.roughness);
    
    float x = 1.0 - max(0, -dot(normalize(LightPosition), currentData.originalNormal));
    vec3 radiance = shade(CameraPosition, vec3(fresnel), data.normal, data.worldPos, LightPosition, abs(LightColor), max(MIN_ROUGHNESS_DIRECT, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - currentData.roughness);    
    
    vec3 difradiance = shadeDiffuse(CameraPosition, data.diffuseColor * (1.0 - fresnel), data.normal, data.worldPos, LightPosition, abs(LightColor), data.roughness, false) * x;
 //   return vec3(0);
    return radiance + difradiance ;
}

vec3 MakeShading(PostProcessingData data){
    return mix(shadingNonMetalic(data), shadingMetalic(data), data.metalness);
}

float AO = 1.0;

vec3 ApplyLighting(PostProcessingData data)
{
    AO = lookupAO(UV, 1.0);
    vec3 result = vec3(0);
    if(LightType == LIGHT_AMBIENT) data.roughness = 1.0;
    vec3 radiance = MakeShading(data);
    
    if(LightUseShadowMap == 1){
        if(LightType == LIGHT_SPOT){
            vec4 lightClipSpace = LightVPMatrix * vec4(data.worldPos, 1.0);
            if(lightClipSpace.z > 0.0){
                vec3 lightScreenSpace = (lightClipSpace.xyz / lightClipSpace.w) * 0.5 + 0.5;   

                float percent = 0;
                if(lightScreenSpace.x >= 0.0 && lightScreenSpace.x <= 1.0 && lightScreenSpace.y >= 0.0 && lightScreenSpace.y <= 1.0) {
                    percent = PCFDeferred(lightScreenSpace.xy, 1.0 - toLogDepth(distance(data.worldPos, LightPosition), LightCutOffDistance));
                }
                result += radiance * (percent);
            }
        } else if(LightType == LIGHT_POINT){
            float target = 1.0 - toLogDepth(distance(data.worldPos, LightPosition), LightCutOffDistance);
            float percent =  max(0, 1.0 - (texture(shadowMapCube, normalize(data.worldPos - LightPosition)).r - target) * 100);
            result += radiance * percent;
        }
        
    } else if(LightUseShadowMap == 0){ 
        result += radiance * AO;
    }
    if(LightType == LIGHT_SPOT) {
        float percent = 1.0;
        vec3 ldir = -quat_mul_vec(LightOrientation, vec3(0,0,-1));
        float dt = -dot(normalize(data.worldPos - LightPosition), ldir);
        float angle =  (cos(LightAngle / 1.41) * 0.5 + 0.5);
        percent = smoothstep(angle, angle + (angle * 0.006), dt);
        result *= max(percent, step(10.0, LightAngle));
    }    
    return result * (1.0 - smoothstep(0.0, LightCutOffDistance, distance(LightPosition, data.worldPos)));
}


vec4 shade(){
    vec4 c = vec4(0);
    if(currentData.cameraDistance > 0){
        vec3 res = ApplyLighting(currentData);
        res *= vec3(sign(LightColor.x), sign(LightColor.y), sign(LightColor.z));
        AO += length(res) * step(0, -dot(res, vec3(-1)));
        c.rgb += clamp(res, 0.0, 1.0);
    }
    c.a = clamp(AO, 0.0, 1.0); 
    return c;
}