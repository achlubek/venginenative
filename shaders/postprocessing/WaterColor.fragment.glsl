#version 430 core

layout(binding = 21) uniform sampler2D waterDistanceTex;
layout(binding = 22) uniform sampler2D inputTex;
layout(binding = 23) uniform sampler2D waterTileTex;

layout(binding = 25) uniform samplerCube coverageDistTex;
layout(binding = 26) uniform samplerCube shadowsTex;
layout(binding = 27) uniform samplerCube skyfogTex;
layout(binding = 24) uniform sampler2D starsTex;
layout(binding = 28) uniform sampler2D moonTex;

#include PostProcessEffectBase.glsl
#include Atmosphere.glsl
#include CSM.glsl

#include WaterHeight.glsl


float fogatt(float dist){
    dist *= 0.000015;
    return min(1.0, (dist * dist) );
}

#define waterdepth 30.0 * WaterWavesScale

vec3 normalx(vec3 pos, float e, float roughness){
    vec2 ex = vec2(e, 0);
    vec3 a = vec3(pos.x, heightwater(pos.xz) * waterdepth, pos.z);    
    vec3 b = vec3(pos.x - e, heightwater(pos.xz - ex.xy) * waterdepth, pos.z);
    vec3 c = vec3(pos.x, heightwater(pos.xz + ex.yx) * waterdepth, pos.z + e);      
    vec3 normal = (cross(normalize(a-b), normalize(a-c)));
    //vec3 p2 = pos * 76.0 + Time;
    return normalize(normal);// + 0.3 * (vec3(noise3d(p2), noise3d(-p2), noise3d(p2.zxy)) * 2.0 - 1.0) * (1.0 - roughness) * (1.0 - roughness)).xyz;
}


#define WaterLevel WaterHeight


#include ResolveAtmosphere.glsl

float hitwater(vec3 pos, inout vec3 dir){
    float planethit = intersectPlane(pos, dir, vec3(0.0, waterdepth + WaterLevel, 0.0), vec3(0.0, -1.0, 0.0));
    float planethit2 = intersectPlane(pos, dir, vec3(0.0, WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
    
    vec3 end = pos + dir * min(abs(planethit), abs(planethit2));
    int steps = 16;
    float stepsf = float(steps);
    float isteps = 1.0 / stepsf;
    float iter = isteps;
    float rd = rand2sTime(UV) * isteps;
    float a = 0.0;
    float w = 0.0;
    for(int i=0;i<steps;i++){
        vec3 u = mix(pos, end, iter + rd);
        float h = WaterLevel + heightwater(u.xz) * waterdepth;
        a +=clamp((u.y - h) / waterdepth, 0.0, 1.0);
        iter += isteps;
    }
    return max(0.0,  a * isteps );
}
float hitwater2(vec3 pos, inout vec3 dir){
    float planethit = intersectPlane(pos, dir, vec3(0.0, waterdepth + WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
    if(planethit < 0.0) return 0.0;
  //  float planethit2 = intersectPlane(pos, dir, vec3(0.0, WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
    planethit = min(planethit, 216.0);
    vec3 end = pos + dir * planethit;
    int steps = 32;
    float stepsf = float(steps);
    float isteps = 1.0 / stepsf;
    float iter = isteps;
    float rd = rand2s(UV * Time) * isteps;
    float a = 1.0;
    float w = 0.0;
    for(int i=0;i<steps - 1;i++){
        vec3 u = mix(pos, end, iter + rd);
        float h = WaterLevel + heightwater(u.xz) * waterdepth;
        a *= (1.0 - isteps * 0.6) + isteps * 0.6 * smoothstep(-0.9, 0.9, u.y - h);
        iter += isteps;
    }
    return clamp(a, 0.0, 1.0);
}

vec4 getLighting(){
    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);

    float hitdist = textureLod(waterDistanceTex, UV, 0.0).r;
   // return vec4(mod(hitdist, 11.0));
    
    if(hitdist < 0.0) return  textureLod(inputTex, UV, 0.0);
    vec3 hitpos = CameraPosition + reconstructCameraSpaceDistance(UV, hitdist);
    
    vec2 px = 1.0 / Resolution; 
    mipmap1 = textureQueryLod(waterTileTex, hitpos.xz * WaterScale *  octavescale1).x;
    float mipmap2 = textureQueryLod(waterTileTex, hitpos.xz * WaterScale *  octavescale1 * 11.2467).x;
    float mipmap3 = textureQueryLod(waterTileTex, hitpos.xz * WaterScale *  octavescale1 * 151.2467).x ;
    float roughness = clamp((pow(mipmap1/ textureQueryLevels(waterTileTex), 1.0)) , 0.0, 0.99);
    mipmap1 *= 0.6  ;
    
    //mipmap1 *= 0.0;
    float height1  = heightwater(hitpos.xz);
    float height2  = heightwaterD(hitpos.xz, 0.5);
    
    vec3 origdir = dir;
    
    vec3 normal = normalx(hitpos, 0.098, roughness);
    
   // return pow(max(0.0, dot(normal, dayData.sunDir)), 10.0) * vec4(1);
  //  return vec4(normal.xyzz * vec4(1,0.2,1,0));
    
  //  normal = normalize(normal + normalx(hitpos * 11.2467, 3.0953, roughness) * 0.3);
   // normal = normalize(normal + normalx(hitpos * 51.2467, 3.0953, roughness) * 0.2);
    normal = normalize(normal + normalx(hitpos * 11.2467, 0.0953, roughness) * 0.4 * (1.0 - mipmap2/ textureQueryLevels(waterTileTex)));
    //normal = normalize(normal + normalx(hitpos * 151.2467, 3.0953, roughness) * 0.1 * (1.0 - mipmap3/ textureQueryLevels(waterTileTex)));
    dir = reflect(origdir, normal);
    dir.y = max(0.05, dir.y);
    float fresnel = getthatfuckingfresnel(mix(0.00, 0.00, roughness), normal, dir, roughness);  
  //  return fresnel * vec4(1);
    //float x =  textureQueryLod(waterTileTex, hitpos.xz * WaterScale *  octavescale1).x / textureQueryLevels(waterTileTex);
    // float h = hitwater2(hitpos, dir) * (1.0 - x) + x;
     
      //  dir = mix(normalize(VECTOR_UP * 2.0 - dir), dir, h);
    /////    fresnel *= mix(0.9, 1.0, h);
     
   vec3 result = vec3(0);

   // roughness = clamp(roughness, 0.0, 1.0);
   // roughness = 1.0 - pow(1.0 - roughness, 2.0);
    vec3 worldpos = hitpos;
    float camdist = hitdist;
   // roughness = 0.91;
    //float whites = max(0.0, noise2d(hitpos.xz*0.1 + Time * 0.1) * 2.0 - 1.0) * noise2d(hitpos.xz*33.0  + Time * 0.3 + noise2d(hitpos.xz*36.0)) * pow(max(0.0, dot(Wind, normal)) * 2.0, 7.0);
  //  whites *= 45.0;
   // return vec4(whites );
            
    PostProceessingData dataReflection = PostProceessingData(
        vec3(1.0),
        normal,
        normalize(cross(
            dir, 
            hitpos
        )).xyz,
        hitpos,
        origdir * camdist,
        camdist,
        roughness,
        1.0
    );
                    
    //result += mix(shadingWater(dataReflection, -dayData.sunDir, getSunColor(0.0), sampleAtmosphere(dir, roughness, 0.0)) * fresnel * 1.0, getSunColor(0.5) * 0.33 * whites, min(1.0, whites));// + sampleAtmosphere(normal, 0.0) * fresnel;
    vec3 atm = sampleAtmosphere(dir, roughness, 0.0, 5);// * (1.0 - roughness * 0.5) * (1.0 - roughness * 0.5);
   // return vec4(atm, 1.0); 
    result +=   shadingWater(dataReflection, normal, -dayData.sunDir, getSunColor(0.0), atm)  * 1.0 * mix(1.0, 1.0, roughness);
    vec3 refr = normalize(refract(origdir, normal, 0.66));
    if(length(currentData.normal) < 0.01) currentData.cameraDistance = 299999.0;        
    float hitdepth = currentData.cameraDistance - hitdist;
    vec3 newposrefracted = hitpos + refr * min(25.0, hitdepth);
    
    PostProceessingData dataRefracted = currentData;
    dataRefracted.roughness = 1.0;
    if(length(dataRefracted.normal) > 0.01) {
        vec3 mixing = vec3(
            1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(hitdepth) * pow(hitdepth, 1.2)),
            1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(hitdepth) * pow(hitdepth, 1.15)),
            1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(hitdepth) * pow(hitdepth, 1.15))
        );
        if(dataReflection.cameraDistance < hitdist) mixing = vec3(1.0);
        vec2 uvX =  abs(projectvdao(newposrefracted));
        
        result += mixing * textureLod(inputTex, uvX, roughnessToMipmap(roughness, inputTex)).rgb * max(0.0, 1.0 - fresnel);
    }
    
    float planethit = intersectPlane(CameraPosition, dir, vec3(0.0, waterdepth + WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
    float planethit2 = intersectPlane(CameraPosition, dir, vec3(0.0, WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));

    vec3 newpos = CameraPosition + dir * planethit;
    vec3 newpos2 = CameraPosition + dir * planethit2;
    
    float ssscoeff = pow(max(0, height1 - height2) * WaterWavesScale * 0.05 * length(WaterScale) , 2.0) ;
    vec3 waterSSScolor = vec3(0.01, 0.33, 0.22) * 0.02 + vec3(0.01, 0.33, 0.22)*  4.71  * ssscoeff  ;
   // result += waterSSScolor * (1.0 - fresnel) * getSunColor(0) ;
   // result += 1.0 - smoothstep(0.002, 0.003, ssscoeff2);
    result += (vec3(0.1,0.19,0.22) + pow(dot(normal,dayData.sunDir) * 0.4 + 0.6,80.0) * vec3(0.8,0.9,0.6) * 0.12) * getSunColor(0) * (1.0 - fresnel)  * 0.069;
    float superscat = pow(max(0.0, dot(refr, dayData.sunDir)), 4.0) * (1.0 - fresnel);
    result += vec3(0.8,0.9,0.6) * pow(max(0.0, height1 - 0.3), 2.0) * 0.0003 * waterdepth * length(WaterScale) * getSunColor(0) * (superscat * 12.0 + 0.2) ;
         
     return  vec4(result, 1.0);
}


vec4 shade(){    
    vec4 color = getLighting();
    return vec4( clamp(color, 0.0, 10.0));
}