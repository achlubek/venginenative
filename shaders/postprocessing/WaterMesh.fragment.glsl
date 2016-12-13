#version 430 core

layout(binding = 23) uniform sampler2D waterTileTex;

#include PostProcessEffectBase.glsl
#include Constants.glsl
#include PlanetDefinition.glsl
uniform float Time;
uniform float WaterSpeed;
#include ProceduralValueNoise.glsl
#include WaterHeight.glsl

uniform float WaterWavesScale;

 

float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{ return dot(point - origin, normal) / dot(direction, normal); }

#define intersects(a) (a >= 0.0)
float mipmapx = 0.2;
float raymarchwater3(vec3 start, vec3 end, int stepsI){
    float stepsize = 1.0 / stepsI;
    float iter = 0;
    vec3 pos = start;
    float h = 0.0;
    float hupper = waterdepth + WaterLevel;
    float hlower = WaterLevel;
    for(int i=0;i<stepsI + 1;i++){
        pos = mix(start, end, iter);
        h = hlower + heightwaterD(pos.xz, mipmapx) * waterdepth;
        if(h > pos.y) {
            return distance(pos, CameraPosition);
        }
        iter += stepsize;
    }
    return -1.0;
}
float raymarchwater2(vec3 start, vec3 end, int stepsI){
    float stepsize = 1.0 / stepsI;
    float iter = 0;
    vec3 pos = start;
    float h = 0.0;
    float hupper = waterdepth + WaterLevel;
    float hlower = WaterLevel;
    for(int i=0;i<stepsI + 1;i++){
        pos = mix(start, end, iter);
        h = hlower + heightwaterD(pos.xz, mipmapx) * waterdepth;
        if(h > pos.y) {
            return raymarchwater3(mix(start, end, iter - stepsize), mix(start, end, iter + stepsize), 6);
        }
        iter += stepsize;
    }
    return -1.0;
}
float raymarchwater(vec3 start, vec3 end, int stepsI){
    float stepsize = 1.0 / stepsI;
    float iter = 0;
    vec3 pos = start;
    float h = 0.0;
    float hupper = waterdepth + WaterLevel;
    float hlower = WaterLevel;
    for(int i=0;i<stepsI + 1;i++){
        pos = mix(start, end, iter);
        h = hlower + heightwaterD(pos.xz, mipmapx) * waterdepth;
        if(h > pos.y) {
            return raymarchwater3(mix(start, end, iter - stepsize), mix(start, end, iter + stepsize), 5);
           // return distance(pos, CameraPosition);
        }
        iter += stepsize;
    }
    return -1.0;
}

float getWaterDistance(){
    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
    
    float planethit = intersectPlane(CameraPosition, dir, vec3(0.0, waterdepth + WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
    float planethit2 = intersectPlane(CameraPosition, dir, vec3(0.0, WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
    bool hitwater = (intersects(planethit) || intersects(planethit2)) && (length(currentData.normal) < 0.01 || currentData.cameraDistance > planethit);
    float dist = -1.0;
    
    if(hitwater){ 
        vec3 newpos = CameraPosition + dir * planethit;
        if(WaterWavesScale > 0.01){
            if(WaterWavesScale < 0.4){
            
                dist = planethit;
            
            } else {
            
                vec3 newpos2 = CameraPosition + dir * planethit2;
                float mult2 = 1.0 - max(0.0, dot(newpos2, -VECTOR_UP));
                float wvw = WaterWavesScale / 10.0;
                int steps = 1 + int((6.0 + mult2 * 24.0) * wvw);
               //
                
              //  if(planethit < 14.0 && planethit > 0.0) steps *= 10;
            //    if(planethit2 < 14.0 && planethit2 > 0.0) steps *= 10;
				mipmapx = textureQueryLod(waterTileTex, newpos.xz * WaterScale *  octavescale1).x;
                if(intersects(planethit) && intersects(planethit2)){
          
                        dist = raymarchwater(newpos, newpos2, steps);
                    
                } else if(intersects(planethit)){
                 //   planethit = min(14999, planethit);
                    dist = raymarchwater(CameraPosition, CameraPosition + dir * planethit, steps);
                } else if(intersects(planethit2)){
                //    planethit2 = min(14999, planethit2);
                    dist = raymarchwater(CameraPosition, CameraPosition + dir * planethit2, steps);
                }
            }
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