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

uniform float WaterHeight;
uniform vec3 Wind;
uniform vec2 WaterScale;
float octavescale1 = 0.0018;
float mipmap1 = 0.0;
float heightwater(vec2 uv){
    return textureLod(waterTileTex, uv * WaterScale * 0.0018, mipmap1).r;
}


float fogatt(float dist){
    dist *= 0.000015;
    return min(1.0, (dist * dist) );
}

#define waterdepth 10.0 * WaterWavesScale

vec3 normalx(vec3 pos, float e, float roughness){
    vec2 ex = vec2(e, 0);
    vec3 a = vec3(pos.x, heightwater(pos.xz) * waterdepth, pos.z);    
    vec3 b = vec3(pos.x + e, heightwater(pos.xz + ex.xy) * waterdepth, pos.z);
    vec3 c = vec3(pos.x, heightwater(pos.xz - ex.yx) * waterdepth, pos.z - e);      
    vec3 normal = (cross(normalize(a-b), normalize(a-c)));
    //vec3 p2 = pos * 76.0 + Time;
    return normalize(normal);// + 0.3 * (vec3(noise3d(p2), noise3d(-p2), noise3d(p2.zxy)) * 2.0 - 1.0) * (1.0 - roughness) * (1.0 - roughness)).xyz;
}


#define WaterLevel WaterHeight


#include ResolveAtmosphere.glsl

vec4 getLighting(){

    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);

    float hitdist = textureLod(waterDistanceTex, UV, 0.0).r;
    if(hitdist < 0.0) return  textureLod(inputTex, UV, 0.0);
    vec3 hitpos = CameraPosition + reconstructCameraSpaceDistance(UV, hitdist);
   // return hitdist * vec4(0.01);
    
    vec2 px = 1.0 / Resolution;
    mipmap1 = textureQueryLod(waterTileTex, hitpos.xz * WaterScale *  octavescale1).x * 0.6;
    float roughness = clamp((pow(mipmap1 / textureQueryLevels(waterTileTex), 1.0)) * WaterWavesScale, 0.0, 0.5) ;
    
    vec3 normal = normalx(hitpos, 0.53, roughness);
    
    vec3 dr2 = reconstructCameraSpaceDistance(UV + vec2(px.x, 0.0), 1.0);
    vec3 dr3 = reconstructCameraSpaceDistance(UV + vec2(0.0, px.y), 1.0);
    
    int smoothsteps = 2;
    float smoothstepsinv = 1.0 / float(smoothsteps);
    
    vec3 dfX = ((CameraPosition + dr2 * hitdist) - hitpos) * smoothstepsinv;
    vec3 dfY = ((CameraPosition + dr3 * hitdist) - hitpos) * smoothstepsinv;

    float smoothedgemult = smoothstep(0.0, 1.1, currentData.cameraDistance - hitdist);

    
    //float roughness = 0.0;
   // roughness = clamp((pow(textureQueryLod(waterTileTex, hitpos.xz * vec2(NoiseOctave2, NoiseOctave3) * octavescale1).x / textureQueryLevels(waterTileTex), 1.0)) * WaterWavesScale, 0.0, 1.0);
  /*  if(WaterWavesScale > 0.01){
        float nw = 1.0;
        for(int x=-smoothsteps; x <= smoothsteps; x++){
            for(int y=-smoothsteps; y <= smoothsteps; y++){
                if(y == 0 && x == 0) continue;
                normal += normalx(hitpos + dfX * float(x) + dfY * float(y), 0.1, roughness);
                nw += 1.0;
            }
        }
        vec3 X = normal / nw;
       // roughness = clamp(length(dfX) + length(dfY), 0.0, 1.0);
       // roughness = 1.0 - min(1.0, length(X) * length(X));
        normal = normalize(X);
    }*/
   // return vec3(roughness);

   vec3 result = vec3(0);

    vec3 origdir = dir;
    dir = reflect(dir, normal);
    dir.y = abs(dir.y);
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
        dir,
        normalize(cross(
            dfX, 
            dfY
        )).xyz,
        hitpos,
        dir * camdist,
        camdist,
        roughness,
        1.0
    );
    float fresnel = getthatfuckingfresnel(mix(0.04, 0.4, roughness), normal, dir, roughness);  
                    
    //result += mix(shadingWater(dataReflection, -dayData.sunDir, getSunColor(0.0), sampleAtmosphere(dir, roughness, 0.0)) * fresnel * 1.0, getSunColor(0.5) * 0.33 * whites, min(1.0, whites));// + sampleAtmosphere(normal, 0.0) * fresnel;
    result += shadingWater(dataReflection, -dayData.sunDir, getSunColor(0.0), sampleAtmosphere(dir, roughness, 0.0, 5)) * fresnel * 1.0 * mix(1.0, 1.0, roughness);
    vec3 refr = normalize(refract(origdir, normal, 0.15));
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
    
 //   float ssscoeff = mix(0.3, 0.3 + 0.7 * pow(1.0 - distance(hitpos, newpos) / distance(newpos, newpos2), 2.0), clamp(WaterWavesScale * 0.5, 0.0, 1.0));
    //ssscoeff *= 1.0 - max(0, dot(origdir, -normal));
    float ssscoeff = 0.1 * (1.0 - fresnel);
    vec3 waterSSScolor = vec3(0.01, 0.49, 0.65) * getDiffuseAtmosphereColor() * 1.2 * (1.0 - fresnel)  * ssscoeff;
    result += waterSSScolor;
    
         
     return vec4(result, 1.0);
}


vec4 shade(){    
    vec4 color = getLighting();
    return vec4( clamp(color, 0.0, 10.0));
}