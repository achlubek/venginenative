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
    vec3 a = xa / max(0.1, Luminence * 0.1);
    a *= Exposure;
    a /= 1.0 + length(a) * 0.4;
    float l = length(a);
    //a = normalize(a) * mix(l, 0.9, 0.2);
    a = pow(a, vec3(Contrast));
    return (rgb_to_srgb(a));

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


uniform float FocalLength;
uniform float LensBlurSize;
float getAmountForDistance(float focus, float dist){

	float f = FocalLength * 0.1;
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
    float focus = textureLod(waterColorTex, vec2(0.5, 0.5), 0.0).a;
    float dist = textureLod(waterColorTex, uv, 0.0).a;
//    return dist * vec3(0.1);
   // if(dist < focus) dist = focus + abs(dist - focus);
    float amountoriginal = getAmountForDistance(focus, dist) * LensBlurSize * 21.019;

    float amount = clamp(amountoriginal, 0.00, 0.017);
    float xda = amount / 0.017;
    if(amount < 0.0005) return fxaa(waterColorTex, UV).rgb;
    //float amount = getAmountForDistance(focus, dist);
    //return vec3(amount / 0.005);
    float stepsize = 6.283185 / 16.0;
    float ringsize = 1.0 / 4.0;
    vec3 sum = vec3(0);
    float weight = 0.001;
    vec2 ratio = vec2(Resolution.y / Resolution.x, 1.0);
    for(float x=0.0;x<6.283185;x+=stepsize){
        for(float y=ringsize;y<1.0;y+=ringsize){
            vec2 displacement = vec2(sin(x + y * 2.54), cos(x + y * 2.54)) * ratio * (y) * amount;
            float distx = textureLod(waterColorTex, uv + displacement, 2.0 * xda).a;
            vec3 c = textureLod(waterColorTex, uv + displacement, 2.0 * xda).rgb;
            float w = (length(c) + 0.3) ;//*   mix(max(0.0, 1.0 - abs(dist - distx)), 1.0, amount * 7.0);// * pow(1.0 - y, 2.0);
            sum += w * c;
            weight += w;
        }
    }
    return sum / weight;

}


vec3 integrateCloudsWater(){
    vec3 color = LensBlurSize > 0.11 ? BoKeH(UV) : texture(waterColorTex, UV).rgb;;
    //vec3 c = applysimplebloom();
    return color;
}

vec4 shade(){
    vec3 color = vec3(0);
    if(CombineStep == STEP_PREVIOUS_SUN){
        color = integrateStepsAndSun();
    } else {
        color = integrateCloudsWater();


        vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
        vec3 dirX = reconstructCameraSpaceDistance(vec2(0.5), 1.0);

        vec2 ss2 = projectvdao(CameraPosition + dayData.sunDir);
        float cloudsonsun = 1.0 - textureLod(resolvedAtmosphereTex, dayData.sunDir, 2.0).a;

        float mindst = step(0.0, dot(dirX, dayData.sunDir)) *
            smoothstep(0.0, 0.05, max(0.0, min(ss2.x,
                min(ss2.y, min(1.0 - ss2.x, 1.0 - ss2.y))
                )));

        vec3 sdirsec = reconstructCameraSpaceDistance(1.0 - ss2, 1.0);
        vec3 sdirthr = reconstructCameraSpaceDistance(((((1.0 - ss2) * 2.0 - 1.0) * 0.5) * 0.5 + 0.5), 1.0);
        vec3 sdirqua = reconstructCameraSpaceDistance(((((ss2) * 2.0 - 1.0) * 0.5) * 0.5 + 0.5), 1.0);

        float secondary = 1.0 - smoothstep(0.06, 0.10, abs(distance(dir, sdirsec)));
        float third = 1.0 - smoothstep(0.06, 0.09, abs(distance(dir, sdirthr)));
        float quad = 1.0 - smoothstep(0.06, 0.08, abs(distance(dir, sdirqua)));

        color += cloudsonsun * getSunColor(0.0) * mindst * secondary * vec3(1.0, 1.0, 0.8) * 0.2;
        color += cloudsonsun * getSunColor(0.0) * mindst * third* vec3(0.6, 0.5, 0.7) * 0.3;
        color += cloudsonsun * getSunColor(0.0) * mindst * quad* vec3(0.5, 0.5, 0.8) * 0.4;

// want super realistic dirt on lens?
        //color += mindst * cloudsonsun * getSunColor(0.0) * pow(max(0.0, dot(dir, dayData.sunDir)), 2.0) * 0.01 * (1.0 - smoothstep(0.26, 0.6, abs(0.5 - supernoise3d(vec3(UV.x, UV.y * (Resolution.y / Resolution.x), 0.0) * 30.0))));
        //color += mindst * cloudsonsun * getSunColor(0.0) * pow(max(0.0, dot(dir, dayData.sunDir)), 2.0) * 0.01 * (1.0 - smoothstep(0.26, 0.6, abs(0.5 - supernoise3d(vec3(UV.x, UV.y * (Resolution.y / Resolution.x), 0.0) * 60.0))));
        color = tonemap(color);
    }
    return vec4( clamp(color, 0.0, 110.0), currentData.cameraDistance * 0.001);
}
