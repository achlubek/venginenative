#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 16) uniform sampler2D inputTex;

#include FXAA.glsl

const float SRGB_ALPHA = 0.055;
uniform float Time;

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
vec3 tonemapX(vec3 x){
    x /= Luminence * 1.0;
    float lumacolor = length(vec3(x.x, x.y, x.z));
    float lumagray = length(x.x);
    
    float mappedgray = x.x*2.0 + x.y *0.5 + x.z * 0.8;
   // mappedgray /= x.x;
    
    vec3 mappedcolor = vec3(log2(x.x + 1.0) / log2(7.0), 
    log2(x.y + 1.0) / log2(7.0), 
    log2(x.z + 1.0) / log2(7.0));
    return mappedcolor;
    
    return mix(mappedgray * 0.08 * vec3(0.8, 0.9, 1.0), mappedcolor, pow(min(1.0, lumacolor*4.1), 4.0));
    
}
vec3 tonemapA(vec3 x){
    vec3 a = Uncharted2Tonemap(x / Luminence);
    vec3 white = vec3(1.0) / Uncharted2Tonemap(vec3(W));
    vec3 c = a * white;
    return rgb_to_srgb(c);
}
float rand2sTime(vec2 co){
    co *= Time;
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
vec3 tonemap(vec3 xa){
    
    vec3 a = xa / max(0.1, Luminence);
    a *= 0.9;
    vec3 x = max(vec3(0.0),a-0.004);
    vec3 retColor = (x*(6.2*x+.5))/(x*(6.2*x+1.7)+0.06);
    vec3 gscale = vec3(retColor.r * 0.7 + retColor.g * 0.25 + retColor.b * 0.2 + rand2sTime(UV) * 0.08) * 0.6;
    return mix(gscale, retColor, 1);//min(1.0, length(retColor)));
    //return retColor;
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

vec4 shade(){    
    vec3 color = texture(inputTex, UV).rgb;
   // vec3 t = textureLod(inputTex, UV, 1.0).rgb;
   // float d = distance(color, t);
  //  color = mix(color, textureLod(inputTex, UV, 2.0).rgb, smoothstep(0.0,  length(vec3(1.0)),d));
    
   // vec3 color = textureLod(inputTex, UV, float(textureQueryLevels(inputTex) - 2.0)).rgb;
   // vec3 color = BoKeH(UV);
   //return vec4(vignette(UV, 1.2, 1.0));
    return vec4(tonemap(color), 1.0);
}