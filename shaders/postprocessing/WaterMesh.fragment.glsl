#version 430 core

layout(binding = 23) uniform sampler2D waterTileTex;

#include PostProcessEffectBase.glsl

uniform float WaterHeight;
uniform float WaterWavesScale;

#define WaterLevel (0.01+ 10.0 * WaterHeight)
#define waterdepth 1.0 * WaterWavesScale

uniform vec2 WaterScale;

float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{ return dot(point - origin, normal) / dot(direction, normal); }

#define intersects(a) (a >= 0.0)

float heightwater(vec2 uv){
    return textureLod(waterTileTex, uv * WaterScale * 0.018, 0.0).r;
}

float raymarchwater(vec3 upper, vec3 lower, int stepsI){
    float stepsize = 1.0 / stepsI;
    float iter = 0;
    vec3 pos = upper;
    float h = 0.0;
    for(int i=0;i<stepsI + 1;i++){
        pos = mix(upper, lower, iter);
        h = heightwater(pos.xz);
        if(h > 1.0 - iter) {
            iter -= stepsize;
            stepsize = stepsize / 4.0;
            for(int g=0;g<4;i++){
                pos = mix(upper, lower, iter);
                h = heightwater(pos.xz);
                if(h > 1.0 - iter) {
                    return distance(pos, CameraPosition);
                }
                iter += stepsize;
            }
            return distance(pos, CameraPosition);
        }
        iter += stepsize;
    }
    return distance(upper, CameraPosition);
}

float getWaterDistance(){
    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
    
    float planethit = intersectPlane(CameraPosition, dir, vec3(0.0, waterdepth + WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
    bool hitwater = intersects(planethit) && (length(currentData.normal) < 0.01 || currentData.cameraDistance > planethit);
    float dist = -1.0;
    
    if(hitwater){ 
        float planethit2 = intersectPlane(CameraPosition, dir, vec3(0.0, WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
        vec3 newpos = CameraPosition + dir * planethit;
        if(WaterWavesScale > 0.01){
            vec3 newpos2 = CameraPosition + dir * planethit2;
            dist = raymarchwater(newpos, newpos2, 1 + int(11.0 * WaterWavesScale));
        } else {
            dist = distance(newpos, CameraPosition);
        }
        if(length(currentData.normal) > 0.01){
            hitwater = currentData.cameraDistance > dist;
        }
    }
    if(hitwater){ 
        return dist;
    }
    return -1.0;
}


vec4 shade(){    
    return vec4(getWaterDistance(), 0.0, 0.0, 0.0);
}