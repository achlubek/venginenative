#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 3) uniform samplerCube skyboxTex;
layout(binding = 5) uniform sampler2D directTex;
layout(binding = 6) uniform sampler2D alTex;
layout(binding = 16) uniform sampler2D aoxTex;

uniform int UseAO;
uniform float T100;
uniform float T001;

#define CLOUD_SAMPLES 18
#define CLOUDCOVERAGE_DENSITY 50    
#include Atmosphere.glsl
#include CSM.glsl
float rand2sx(vec2 co){
        return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

#define WaterLevel (0.01+ 10.0 * NoiseOctave6)
vec3 tracegodrays(vec3 p1, vec3 p2){
    float rays = 0.0;
    float rays2 = 0.0;
    float coverage = 1.0;
    int steps = int(clamp(floor(distance(p1, p2) * 0.2), 17.0, 32.0));
    float stepsize = 1.0 / float(steps);
    float rd = rand2s(UV * Time) * stepsize;
    float iter = 0.0;
    vec3 lastpos = p1;
    float floorh = WaterLevel;
    float highh = 100.0 * NoiseOctave8;
    float cloudslow = planetradius + floorh;
    float cloudshigh = planetradius + highh;
    vec3 d = normalize(dayData.sunDir);
    float w = 0.0;
    for(int i=0;i<steps;i++){
        vec3 p = mix(p1, p2, rd + iter);
        Ray r = Ray(vec3(0, planetradius, 0) + p, d);
       // float planethit = max(0, sign(-rsi2(r, planet)));
        float dist = distance(p, lastpos);
        float height = length(vec3(0, planetradius, 0) + p);
        lastpos = p;
        float xz  = (1.0 - smoothstep(cloudslow, cloudshigh, height));
        float z = mix(noise3d(p.xyz*0.015 + Time * 0.1), 1.0, pow(xz, 3.0));
        rays += coverage * (dist * CSMQueryVisibilitySimple(p)  ) * (1.0 - xz);
        rays2 += coverage * (dist) * (1.0 - xz);
        w += coverage;
        coverage *= 1.0 - max(dist * mix(z, 1.0, xz) * NoiseOctave5 * 0.00004, 0.0);
        if(coverage <= 0.0) break;
        iter += stepsize;
    }
    return vec3(clamp(rays / w, 0.0, 66.0), clamp(rays2 / w, 0.0, 66.0), 1.0 - clamp(coverage, 0.0, 1.0));
}

#define waterdepth 1.0 * WaterWavesScale
vec3 gimmegodrays(vec3 o, vec3 d){
    vec3 rays = vec3(0.0);

    float floorh = WaterLevel;
    float highh = 100.0 * NoiseOctave8;
    
    float cloudslow = planetradius + floorh;
    float cloudshigh = planetradius + highh;
    
    Ray r = Ray(vec3(0, planetradius, 0) + o, d);
    float height = length(vec3(0, planetradius, 0) + o);
    
    sphere1 = Sphere(vec3(0), planetradius + floorh);
    sphere2 = Sphere(vec3(0), planetradius + highh);
        
    float planethit = rsi2(r, planet);
    float hitfloor = rsi2(r, sphere1);
    float floorminhit = minhit;
    float floormaxhit = maxhit;
    float hitceil = rsi2(r, sphere2);
    float ceilminhit = minhit;
    float ceilmaxhit = maxhit;
    float dststart = 0.0;
    float dstend = 0.0;
    float lim = length(currentData.normal) < 0.1 ? 999999.0 :currentData.cameraDistance ;
    //return lim * 0.1;
    planethit = min(lim , planethit);
    hitfloor = min(lim, hitfloor);
    hitceil = min(lim, hitceil);
    floorminhit = min(lim, floorminhit);
    floormaxhit = min(lim, floormaxhit);
    ceilminhit = min(lim, ceilminhit);
    ceilmaxhit = min(lim, ceilmaxhit);
    minhit = min(lim, minhit);
    maxhit = min(lim, maxhit);
    if(height < cloudslow){
        if(planethit < 0){
            rays = tracegodrays(o + d * hitfloor, o + d * hitceil);
        }
    } else if(height >= cloudslow && height < cloudshigh){
        if(intersects(hitfloor)){
            rays = tracegodrays(o, o + d * floorminhit);
        } else {
            rays = tracegodrays(o, o + d * hitceil);
        }
    } else if(height >= cloudshigh){
        if(!intersects(hitfloor) && !intersects(hitceil)){
            rays = vec3(0.0);
        } else if(!intersects(hitfloor)){
            rays = tracegodrays(o + d * minhit, o + d * maxhit);
        } else {
            rays = tracegodrays(o + d * ceilminhit, o + d * floorminhit);
        }
    }
    return rays;
}

vec4 shade(){  
//    vec2 pixels = 1.0 / Resolution;
//    pixels *= 0.5;
 //   currentData = loadData(UV + pixels);
    if(currentData.cameraDistance < 0.01) currentData.cameraDistance = 999999.0;
    vec3 color = NoiseOctave5 > 0.011 ? (gimmegodrays(CameraPosition, normalize(reconstructCameraSpaceDistance(UV, 1.0) ))) : vec3(0.0);
    return vec4( color.x, color.y, color.z, 1.0);
}