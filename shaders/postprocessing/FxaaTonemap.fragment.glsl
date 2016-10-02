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


float InputFocalLength = 35.0;
float LensBlurAmount = 1.00;
float getAmountForDistance(float focus, float dist){

	float f = InputFocalLength;
	float d = focus*1000.0; //focal plane in mm
	float o = dist*1000.0; //depth in mm
	
	float fstop = 64.0 / LensBlurAmount;
	float CoC = 1.0;
	float a = (o*f)/(o-f); 
	float b = (d*f)/(d-f); 
	float c = (d-f)/(d*fstop*CoC); 
	
	float blur = abs(a-b)*c;
	return blur;
}

vec3 BoKeH(vec2 uv){
    float focus = texture(inputTex, vec2(0.5, 0.5)).a * 10.0;
    float dist = texture(inputTex, uv).a * 10.0;
    if(dist < focus) dist = focus + abs(dist - focus);
    float amountoriginal = clamp(getAmountForDistance(focus, dist), 0.0, 1.0);
    float amount = amountoriginal * 0.019;
    //float amount = getAmountForDistance(focus, dist);
    //return vec3(amount / 0.005);
    float stepsize = 6.283185 / 16.0;
    float ringsize = 1.0 / 6.0;
    vec3 sum = vec3(0);
    float weight = 0.0;
    vec2 ratio = vec2(Resolution.y / Resolution.x, 1.0);
    for(float x=0.0;x<6.283185;x+=stepsize){
        for(float y=ringsize;y<1.0;y+=ringsize){
            vec2 displacement = vec2(sin(x + y * 2.1), cos(x + y * 2.1)) * ratio * (y) * amount;
            vec3 c = textureLod(inputTex, uv + displacement, 4.0 * amountoriginal).rgb;
            float w = length(c) + 0.3;
            sum += w * c;
            weight += w;
        }
    }
    return sum / weight;
}

vec3 Uncharted2Tonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}
layout (std430, binding = 0) buffer R1
{
  float Luminence; 
}; 
vec3 tonemap(vec3 x){
    vec3 a = Uncharted2Tonemap(1.0 * x / Luminence);
    vec3 white = vec3(1.0) / Uncharted2Tonemap(vec3(W));
    vec3 c = a * white;
    return rgb_to_srgb(c);
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
   // vec3 color = textureLod(inputTex, UV, float(textureQueryLevels(inputTex) - 2.0)).rgb;
   // vec3 color = BoKeH(UV);
    return vec4(tonemap(color), 1.0);
}