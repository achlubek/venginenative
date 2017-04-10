#version 430 core

#define STEP_PREVIOUS_SUN 0
#define STEP_WATER_CLOUDS 1

#include Constants.glsl
#include PlanetDefinition.glsl
#include PostProcessEffectBase.glsl
#include Atmosphere.glsl
#include noise2D.glsl
#include FXAA.glsl
#include ResolveAtmosphere.glsl


vec2 project_above(vec3 pos){
    vec2 a = ((pos - CameraPosition).xz / AboveSpan) * 0.5 + 0.5;
    a.y = 1.0 - a.y;
    a.x = 1.0 - a.x;
    return a;
}

float rdhashx2 = 0.453451 + Time;
vec2 randpoint2(){
    float x = rand2s(UV * rdhashx2);
    rdhashx2 += 2.1231255;
    float y = rand2s(UV * rdhashx2);
    rdhashx2 += 1.6271255;
    return vec2(x, y) * 2.0 - 1.0;
}

float getAboveAO(vec3 p){
    //AboveAO
    vec2 aboveuv = project_above(p);
    float sum = 0.0;
    float ws = 0.0;
    for(int i=0;i<16;i++){
        float v = textureLod(aboveViewDataTex, aboveuv + randpoint2() * 0.015, 0.0).r;
        sum +=  clamp(max(0.0, v - p.y), 0.0, 1.0);
        ws += 1.0;
    }
    return 1.0 - sum / ws;
}

vec4 applyAirLayer(vec3 dir, vec3 colorLit, vec3 colorDiff, float height, float maxdist){
    vec3 volumetrix = vec3(0.0);
    float volumetrix2 = 0.0;
    int steps = 16;
    float stepsize = 1.0 / float(steps);
    float rd = rand2sTime(UV);
    float iter = rd * stepsize;
    vec3 start = CameraPosition;
    float theheight = rsi2(Ray(toplanetspace(CameraPosition), dir), Sphere(vec3(0.0), planetradius + height));
    float atmceil = 0.0;
    if(length(toplanetspace(CameraPosition)) > planetradius + height) {
        start = CameraPosition + dir * theheight;
        atmceil = rsi2(Ray(toplanetspace(CameraPosition), dir), Sphere(vec3(0.0), planetradius));
        if(atmceil < 0.0) return vec4(0.0);
    } else {
        atmceil = theheight;
    }
    float waterfloor = rsi2(Ray(toplanetspace(CameraPosition), dir), Sphere(vec3(0.0), planetradius));
    float LN = length(currentData.normal);
    vec3 realpos = mix(CameraPosition + dir * (waterfloor > 0.0 ? waterfloor : atmceil), currentData.worldPos, step(0.01, LN));
    vec3 end = realpos;
    float point_full_coverage = maxdist;
    float overall_coverage = 1.0;
    float distanceweight =  distance(start, end) * 0.1;
    float coveragepart = linearstep(0.0, point_full_coverage, distance(start, end))   ;
//    coveragepart*=coveragepart;
    float dirdotsun = dot(dir, dayData.sunDir) * 0.5 + 0.5;
    float stdr = (1.0 - smart_inverse_dot(1.0 - dirdotsun, 2.0));
    float sw = 0.0;
    vec3 realst = start;
    float maxdst = min(point_full_coverage, distance(start, end));
    float densiti =  (1.0 / (point_full_coverage * 0.001 + 1.0)) * distance(start, end) * 0.01;
    //return coveragepart * (1.0 - smart_inverse_dot(max(0.0, dot(dir, VECTOR_UP)), 11.0)) ;

    vec3 closeend = start + dir * min(maxdist, distance(start, end));
    float vissum = 0.0;
    float ambient = 0.0;
    for(int i=0;i<steps;i++){
            vec3 p = mix(start, closeend, iter);
            vissum += clamp(CSMQueryVisibilitySimple(p), 0.0, 1.0);
            ambient += getAboveAO(p);
            iter += stepsize;
    }
    vissum *= stepsize;
    ambient *= stepsize;
    iter = 0.0;

    for(int i=0;i<steps;i++){
        vec3 p = mix(start, end, iter);
        float coverage = densiti * (1.0 - linearstep(0.0, height, p.y));
        volumetrix += (vissum * colorLit  + colorDiff * ambient) * overall_coverage;
        sw += overall_coverage;
        overall_coverage -= coverage;
        if(overall_coverage < 0.0) break;

        iter += stepsize;
    }
    overall_coverage = clamp(overall_coverage, 0.0, 1.0);
    volumetrix /= max(0.01, sw);
    //color = mix(color + min(coveragex, 1.0) * atma * pow(volumetrix2, 6.0), atma * volumetrix2, length(currentData.normal) > 0.01 ? min(coveragex, 1.0) : 0.0);
    //if(LN > 0.01 || waterfloor > 0.0) color = mix(clamp(color, 0.0, 10000.0), volumetrix * clamp(volumetrix2 * stepsize, 0.0, 1.0) * stepsize, overall_coverage);
    //else color *=pow( stepsize * volumetrix2, 1.0);
    return vec4(volumetrix, (1.0 - overall_coverage));//sqrt(log2(overall_coverage + 1.0) / log2(1.0 + 13.0)));
}
vec4 shade(){
    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
    float SunDT = max(0.0, dot(dayData.sunDir, VECTOR_UP));
    float SDDT = 0.5 + 0.5 * dot(dayData.sunDir, dir);
    vec3 AtmDiffuse = getDiffuseAtmosphereColor();
    vec3 direct_color = getSunColorDirectly(0.0) *0.2* (1.0 - smart_inverse_dot(1.0 - SDDT, 12.0)) * FogColor;
    vec3 diffuse_color = (AtmDiffuse ) * 0.45 * FogColor;

    vec4 air = applyAirLayer(dir, direct_color, diffuse_color, FogHeight, FogMaxDistance);
    return air;
}
