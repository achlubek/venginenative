#version 430 core


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

uniform int UseAO;

uniform int CombineStep;
#define STEP_PREVIOUS_SUN 0
#define STEP_WATER_CLOUDS 1

#include Constants.glsl
#include PlanetDefinition.glsl
#include PostProcessEffectBase.glsl
#include Atmosphere.glsl
#include noise2D.glsl
#include CSM.glsl
#include FXAA.glsl
#include ResolveAtmosphere.glsl

const float SRGB_ALPHA = 0.055;
float linear_to_srgb(float channel) {
    if(channel <= 0.0031308)
    return 12.92 * channel;
    else
    return (1.0 + SRGB_ALPHA) * pow(channel, 1.0/2.4) - SRGB_ALPHA;
}

vec3 rgb_to_srgb(vec3 rgb) {
    return vec3(
    linear_to_srgb(rgb.r),
    linear_to_srgb(rgb.g),
    linear_to_srgb(rgb.b)
    );
}

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;


vec3 Uncharted2Tonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}
layout (std430, binding = 0) buffer R1
{
  float Luminence;
};
uniform float Exposure;
uniform float Contrast;

vec3 testmap(vec3 c){
    return vec3(
         1.879574*c.r  -  1.03263*c.g  + 0.153055*c.b,
         -0.21962*c.r  + 1.715146*c.g -  0.49553* c.b,
         0.006956*c.r  -  0.51487*c.g  + 1.507914*c.b
    );
}

vec3 tonemap(vec3 xa){

    vec3 a = xa / max(0.1, Luminence * 0.07);
    a *= Exposure;
    a /= 1.0 + length(a) * 0.4;
    return testmap(rgb_to_srgb(a));

}

//vec3 myfuckingperfecttonemap(vec3 a){
//    float luma = length(vec3(a.r, a.g * 1.5, a.b));
//}

float vignette(vec2 uv, float scale, float strength){
    float d = length(uv * 2.0 - 1.0) * scale;
    return (1.0 - max(0.0, d - 1.0)) * strength + (1.0 - strength);
}


/*
float edgeDetect(vec2 uv){
    vec2 pixel = 1.0 / Resolution;
    vec2 crs[] = vec2[](
        vec2(pixel.x, pixel.y),
        vec2(-pixel.x, pixel.y),
        vec2(pixel.x, -pixel.y),
        vec2(-pixel.x, -pixel.y),
        vec2(pixel.x, 0.0),
        vec2(0.0, pixel.y),
        vec2(-pixel.x, 0.0),
        vec2(0.0, -pixel.y)
    );

}*/

vec3 integrateStepsAndSun(){
    currentData.cameraDistance = length(currentData.normal) < 0.01 ? 999999.0 : currentData.cameraDistance;
    return getNormalLighting(UV, currentData);
}

vec3 integrateCloudsWater(){
    return tonemap(texture(waterColorTex, UV).rgb);
}

vec4 shade(){
    vec3 color = vec3(0);
    if(CombineStep == STEP_PREVIOUS_SUN){
        color = integrateStepsAndSun();
    } else {
        color = integrateCloudsWater();
    }
    return vec4( clamp(color, 0.0, 110.0), currentData.cameraDistance * 0.001);
}
