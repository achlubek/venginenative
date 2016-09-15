#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 16) uniform sampler2D inputTex;

#include FXAA.glsl

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

vec3 tonemap(vec3 x){
    vec3 a = Uncharted2Tonemap(0.5 * x);
    vec3 white = vec3(1.0) / Uncharted2Tonemap(vec3(W));
    vec3 c = a * white;
    return rgb_to_srgb(c);
}

vec4 shade(){    
    //vec3 color = fxaa(inputTex, UV).rgb;
    vec3 color = texture(inputTex, UV).rgb;
    return vec4(tonemap(color), 1.0);
}