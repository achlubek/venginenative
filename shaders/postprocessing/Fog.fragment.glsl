#version 430 core


layout(binding = 3) uniform samplerCube skyboxTex;
layout(binding = 5) uniform sampler2D directTex;
layout(binding = 6) uniform sampler2D alTex;
//layout(binding = 16) uniform sampler2D aoxTex;
//layout(binding = 20) uniform sampler2D fogTex;
layout(binding = 15) uniform sampler2D waterFoamTex;
layout(binding = 21) uniform sampler2D waterColorTex;
layout(binding = 22) uniform sampler2D inputTex;
layout(binding = 23) uniform sampler2D aboveViewDataTex;

layout(binding = 25) uniform samplerCube coverageDistTex;
layout(binding = 26) uniform samplerCube shadowsTex;
//layout(binding = 27) uniform samplerCube skyfogTex;
layout(binding = 28) uniform sampler2D moonTex;
layout(binding = 23) uniform sampler2D waterTileTex;
layout(binding = 24) uniform sampler2D starsTex;
layout(binding = 29) uniform samplerCube resolvedAtmosphereTex;
layout(binding = 31) uniform sampler2D sunRSMTex;
layout(binding = 34) uniform sampler2D sunRSMWPosTex;
layout(binding = 33) uniform sampler2D sunRSMNormTex;

uniform int UseAO;

uniform int CombineStep;

uniform mat4 SunRSMVPMatrix;
#define STEP_PREVIOUS_SUN 0
#define STEP_WATER_CLOUDS 1

#include Constants.glsl
#include PlanetDefinition.glsl
#include PostProcessEffectBase.glsl
#include Atmosphere.glsl
#include noise2D.glsl
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
    float luma = length(a);
    vec3 xa2 = xa + 0.00001;
    vec3 coloressence = normalize(xa2);
    //a /= 1.0 + log(luma) * 1.1;
    //a = coloressence * mix(sqrt(clamp(luma, 0.0001, 99999.0)), luma, 0.5);
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
    float xda = amount / 0.011;
    if(amount < 0.0005) return fxaa(waterColorTex, UV).rgb;
    //float amount = getAmountForDistance(focus, dist);
    //return vec3(amount / 0.005);
    float stepsize = 6.283185 / 16.0;
    float ringsize = 1.0 / 4.0;
    vec3 sum = vec3(0);
    float weight = 0.001;
    vec2 ratio = vec2(Resolution.y / Resolution.x, 1.0);
    float rd = rand2sTime(UV);
    for(float x=0.0;x<6.283185;x+=stepsize){
        for(float y=ringsize * rd;y<1.0;y+=ringsize){
            vec2 displacement = vec2(sin(x + y * 2.54), cos(x + y * 2.54)) * ratio * (y) * amount;
            float distx = textureLod(waterColorTex, uv + displacement, 2.0 * xda).a;
            vec3 c = textureLod(waterColorTex, uv + displacement, 2.0 * xda).rgb;
            float w = (length(c) + 0.3) * (1.0 - y);//*   mix(max(0.0, 1.0 - abs(dist - distx)), 1.0, amount * 7.0);// * pow(1.0 - y, 2.0);
            sum += w * c;
            weight += w;
        }
    }
    return sum / weight;

}

#include Quaternions.glsl

uniform int ShowSelection;
uniform vec3 SelectionPos;
uniform vec4 SelectionQuat;

vec3 showSelection(vec3 dir){
    Sphere sp = Sphere(SelectionPos, 0.5);
    Ray r = Ray(CameraPosition, dir);
    float primdst = rsi2(r, sp);
    vec2 minmax = vec2(minhit, maxhit);
    float outputMult = step(0.0, primdst);
    float cdst = currentData.cameraDistance;
    cdst += 99999.0 * (1.0 - step(0.01, cdst));
    minmax.y = min(minmax.y, cdst);
    minmax.x = min(minmax.x, cdst);
    float middle = (minmax.y - minmax.x);
    float dstmlt = 1.0 - smoothstep(0.0, 1.0, distance(r.o + r.d * middle, sp.pos));
    vec3 c = outputMult * vec3(0.6, 0.7, 1.0) * (minmax.y - minmax.x);
    c += shade_ray_data(currentData, -normalize(currentData.worldPos - SelectionPos), vec3(1.0, 1.0, 2.0) * 0.1);
    // * (minmax.y - minmax.x);
    /*
    vec3 n = normalize((r.o + r.d * minmax.x) - SelectionPos);

    vec3 dirx = vec3(1.0, 0.0, 0.0);
    vec3 diry = vec3(0.0, 1.0, 0.0);
    vec3 dirz = vec3(0.0, 0.0, 1.0);

    dirx = quat_mul_vec(SelectionQuat, dirx);
    diry = quat_mul_vec(SelectionQuat, diry);
    dirz = quat_mul_vec(SelectionQuat, dirz);

    float dtx = pow(abs(dot(dirx, n)), 23.0);
    float dty = pow(abs(dot(diry, n)), 23.0);
    float dtz = pow(abs(dot(dirz, n)), 23.0);*/

    //return outputMult * (c + vec3(dtx, dty, dtz) * 2.0);
    return (c * 7.0);
}

vec3 integrateCloudsWater(){
    vec3 color = LensBlurSize > 0.11 ? BoKeH(UV) : texture(waterColorTex, UV).rgb;;
    //vec3 c = applysimplebloom();
    return color;
}

float lenssun(vec3 dir){
    //return smoothstep(0.997, 0.999, dot(dir, dayData.sunDir));
    vec3 sdir = dayData.sunDir;
    vec2 ss1 = projectvdao(CameraPosition + dir);
    vec2 ss2 = projectvdao(CameraPosition + sdir);
    ss1.x *= Resolution.x / Resolution.y;
    ss2.x *= Resolution.x / Resolution.y;

    vec3 differente = normalize(dir - sdir) * 4.0;
    //return smoothstep(0.997, 0.998, dot(dir, dayData.sunDir));// fuck it pow(1.0 / (distance(dir, dayData.sunDir) * 22.0 - 0.05), 5.0);
    float primary = pow(1.0 / abs((distance(dir, sdir) * 22.0 - 0.05)), 3.0 + supernoise3dX(differente * 1.3 + Time * 0.2) * 1.0);

    return primary;
}

float rdhashx2 = 0.453451 + Time;
vec2 randpoint2(){
    float x = rand2s(UV * rdhashx2);
    rdhashx2 += 2.1231255;
    float y = rand2s(UV * rdhashx2);
    rdhashx2 += 1.6271255;
    return vec2(x, y) * 2.0 - 1.0;
}

vec2 projectsunrsm(vec3 pos){
    vec4 tmp = (SunRSMVPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}

uniform float WaterSpeed;
#include WaterHeight.glsl

uniform vec3 FogColor;
uniform float FogMaxDistance;
uniform float FogHeight;
float fbmHId(vec3 p){
    p *= 0.05;
	//p += getWind(p * 0.2) * 6.0;
	float a = 0.0;
    float w = 1.0;
    float wc = 0.0;
	for(int i=0;i<3;i++){
        //p += noise(vec3(a));
		a += clamp(2.0 * abs(0.5 - (supernoise3dX(p))) * w, 0.0, 1.0);
		wc += w;
        w *= 0.6;
		p = p * 3.0;
	}
	return a / wc;// + noise(p * 100.0) * 11;
}
vec4 applyAirLayer(vec3 dir, vec3 colorLit, vec3 colorDiff, float height, float maxdist){
    vec3 volumetrix = vec3(0.0);
    float volumetrix2 = 0.0;
    int steps = 7;
    float stepsize = 1.0 / 7.0;
    float rd = rand2sTime(UV);
    float iter = rd * stepsize;
    vec3 start = CameraPosition;
    float theheight = rsi2(Ray(toplanetspace(CameraPosition), dir), Sphere(vec3(0.0), planetradius + height));
    float atmceil = 0.0;
    if(minhit > 0.0 && maxhit > 0.0) {
        start += dir * theheight;
        atmceil = rsi2(Ray(toplanetspace(CameraPosition), dir), Sphere(vec3(0.0), planetradius));
        if(atmceil < 0.0) return vec4(0.0);
    } else {
        atmceil = theheight;
    }
    float waterfloor = rsi2(Ray(toplanetspace(CameraPosition), dir), Sphere(vec3(0.0), planetradius));
    float LN = length(currentData.normal);
    vec3 realpos = mix(CameraPosition + dir * (waterfloor > 0.0 ? waterfloor : atmceil), currentData.worldPos, step(0.01, LN));
    vec3 end = realpos;
    //float maxdst = min(FogMaxDistance, distance(start, end));
    //end = start + dir * maxdst;
    float point_full_coverage = maxdist;
    float overall_coverage = 1.0;
    float coveragepart = linearstep(0.0, point_full_coverage, distance(start, end)) * stepsize * 6.0;
//    coveragepart*=coveragepart;
    float dirdotsun = dot(dir, dayData.sunDir) * 0.5 + 0.5;
    float stdr = (1.0 - smart_inverse_dot(1.0 - dirdotsun, 2.0));
    float sw = 0.0;
    vec3 realst = start;
    for(int i=0;i<steps;i++){
        vec3 p = mix(start, end, iter );
        float coverage = coveragepart * (1.0 - linearstep(0.0, height, p.y));// * fbmHId(p * (0.95 + 0.05 * supernoise3dX(p * 0.006 + vec3(0.0, Time * 0.07, 0.0)))  + Time ) ;
        float vis = clamp(CSMQueryVisibilitySimple(p), 0.0, 1.0);
        volumetrix += (vis * colorLit  + colorDiff ) * max(0.0, 1.0 - overall_coverage) ;
        sw += max(0.0, 1.0 - overall_coverage) ;
        overall_coverage *= 1.0 - coverage;
        //if(overall_coverage >= 1.0) break;

        iter += stepsize;
    }
    volumetrix /= sw;
    //color = mix(color + min(coveragex, 1.0) * atma * pow(volumetrix2, 6.0), atma * volumetrix2, length(currentData.normal) > 0.01 ? min(coveragex, 1.0) : 0.0);
    //if(LN > 0.01 || waterfloor > 0.0) color = mix(clamp(color, 0.0, 10000.0), volumetrix * clamp(volumetrix2 * stepsize, 0.0, 1.0) * stepsize, overall_coverage);
    //else color *=pow( stepsize * volumetrix2, 1.0);
    return vec4(volumetrix * 1.0, sqrt(1.0 - overall_coverage));//sqrt(log2(overall_coverage + 1.0) / log2(1.0 + 13.0)));
}
vec4 shade(){
    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
    float SunDT = max(0.0, dot(dayData.sunDir, VECTOR_UP));
    float SDDT = 0.5 + 0.5 * dot(dayData.sunDir, dir);
    vec3 AtmDiffuse = getDiffuseAtmosphereColor();
    vec3 direct_color = getSunColorDirectly(0.0) * 6.9 * (1.0 - smart_inverse_dot(1.0 - SDDT, 12.0)) * smart_inverse_dot(SunDT, 2.0) * FogColor;
    vec3 diffuse_color = (AtmDiffuse * 0.2 ) * 0.8 * FogColor;

    vec4 air = applyAirLayer(dir, direct_color, diffuse_color, FogHeight, FogMaxDistance);
    return air;
}
