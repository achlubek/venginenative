#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 16) uniform sampler2D inputTex;

uniform float Time;

float rand2sTime(vec2 co){
    co *= Time;
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

uniform float FocalLength;
uniform float LensBlurSize;
float getAmountForDistance(float focus, float dist){

	float f = FocalLength;
	float d = focus*1000.0; //focal plane in mm
	float o = dist*1000.0; //depth in mm
	
	float fstop = 64.0;
	float CoC = 1.0;
	float a = (o*f)/(o-f); 
	float b = (d*f)/(d-f); 
	float c = (d-f)/(d*fstop*CoC); 
	
	float blur = abs(a-b)*c;
	return blur;
}

vec3 BoKeH(vec2 uv){
    float focus = textureLod(inputTex, vec2(0.5, 0.5), 0.0).a;
    float dist = textureLod(inputTex, uv, 0.0).a;
   // if(dist < focus) dist = focus + abs(dist - focus);
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

vec4 shade(){    
    //vec3 color = fxaa(inputTex, UV).rgb;
    vec3 color = LensBlurSize > 0.11 ? BoKeH(UV) : textureLod(inputTex, UV, 0.0).rgb;
    return vec4(color, 1.0);
}