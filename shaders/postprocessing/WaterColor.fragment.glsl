#version 430 core

layout(binding = 21) uniform sampler2D waterDistanceTex;
layout(binding = 22) uniform sampler2D inputTex;
layout(binding = 23) uniform sampler2D waterTileTex;

layout(binding = 25) uniform samplerCube coverageDistTex;
layout(binding = 26) uniform samplerCube shadowsTex;
layout(binding = 27) uniform samplerCube skyfogTex;

#include PostProcessEffectBase.glsl
#include Atmosphere.glsl
#include CSM.glsl

uniform float WaterHeight;
uniform vec3 Wind;
uniform vec2 WaterScale;
float octavescale1 = 0.018;
float mipmap1 = 0.0;
float heightwater(vec2 uv){
    return textureLod(waterTileTex, uv * WaterScale * 0.018, mipmap1).r;
}

float roughnessToMipmap(float roughness, samplerCube txt){
    //roughness = roughness * roughness;
    float levels = max(0, float(textureQueryLevels(txt)));
    float mx = log2(roughness*1024+1)/log2(1024);
    return mx * levels;
}
float roughnessToMipmap(float roughness, sampler2D txt){
    //roughness = roughness * roughness;
    float levels = max(0, float(textureQueryLevels(txt)));
    float mx = log2(roughness*1024+1)/log2(1024);
    return mx * levels;
}
vec4 smartblur(vec3 dir, float roughness){
    float levels = max(0, float(textureQueryLevels(cloudsCloudsTex)));
    float mx = log2(roughness*1024+1)/log2(1024);
    float mlvel = mx * levels;
    vec4 ret = vec4(0);
    ret.xy = textureLod(coverageDistTex, dir, mlvel).rg;
    ret.z = textureLod(shadowsTex, dir, mlvel).r;
    //ret.z = blurshadows(dir, roughness);
    ret.w = textureLod(skyfogTex, dir, mlvel).r;
   // ret.w = blurskyfog(dir, roughness);
    return ret;
    
}



float fogatt(float dist){
    dist *= 0.000015;
    return min(1.0, (dist * dist) );
}

#define waterdepth 1.0 * WaterWavesScale

vec3 normalx(vec3 pos, float e, float roughness){
    vec2 ex = vec2(e, 0);
    vec3 a = vec3(pos.x, heightwater(pos.xz) * waterdepth, pos.z);    
    vec3 b = vec3(pos.x + e, heightwater(pos.xz + ex.xy) * waterdepth, pos.z);
    vec3 c = vec3(pos.x, heightwater(pos.xz - ex.yx) * waterdepth, pos.z - e);      
    vec3 normal = (cross(normalize(a-b), normalize(a-c)));
    //vec3 p2 = pos * 76.0 + Time;
    return normalize(normal);// + 0.3 * (vec3(noise3d(p2), noise3d(-p2), noise3d(p2.zxy)) * 2.0 - 1.0) * (1.0 - roughness) * (1.0 - roughness)).xyz;
}


#define WaterLevel (0.01+ 10.0 * NoiseOctave6)

float getthatfuckingfresnel(float reflectivity, vec3 normal, vec3 cameraSpace, float roughness){
    vec3 dir = normalize(reflect(normalize(cameraSpace), normal));
    float base =  1.0 - abs(dot(normalize(normal), dir));
    float fresnel = (reflectivity + (1-reflectivity)*(pow(base, mix(5.0, 0.8, roughness))));
    return fresnel ;
}

vec3 shadingMetalic(PostProceessingData data, vec3 lightDir, vec3 color){
    float fresnelR = getthatfuckingfresnel(data.diffuseColor.r, data.normal, data.cameraPos, data.roughness);
    float fresnelG = getthatfuckingfresnel(data.diffuseColor.g, data.normal, data.cameraPos, data.roughness);
    float fresnelB = getthatfuckingfresnel(data.diffuseColor.b, data.normal, data.cameraPos, data.roughness);
    float fresnel = getthatfuckingfresnel(0.04, data.normal, normalize(data.cameraPos), data.roughness);
    vec3 newBase = vec3(fresnelR, fresnelG, fresnelB);
    //   return vec3(fresnel);
    float x = 1.0 - max(0, dot(lightDir, data.originalNormal));
    return shade(CameraPosition, newBase, data.normal, data.worldPos, data.worldPos - lightDir * 1.0, color,  max(0.004, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - data.roughness);
}
float sun(vec3 dir, vec3 sundir, float gloss, float ansio){
    float dt = max(0, dot(dir, sundir));
    float dt2 = max(0, dot(normalize(vec3(dir.x, abs(sundir.y), dir.z)), sundir));
    float dty = 1.0 - max(0, abs(dir.y - sundir.y));
    dt = mix(dt, dt2, ansio);
    //return dt;
    return pow(dt*dt*dt*dt, 112.0 * gloss*gloss + 1.0) * (200.0 * gloss + 1.0) * smoothstep(-0.02, -0.01, sundir.y);
   // return smoothstep(0.997, 1.0, dt);
}
vec3 shadingWater(PostProceessingData data, vec3 lightDir, vec3 colorA, vec3 colorB){
    float fresnel = getthatfuckingfresnel(0.04, data.normal, normalize(data.cameraPos), 0.0);
    return sun(data.normal, SunDirection, 1.0 - data.roughness , data.roughness) * 25.0 * colorA * fresnel + colorB * (1.0 - fresnel);
}

vec3 shadingNonMetalic(PostProceessingData data, vec3 lightDir, vec3 color){
    float fresnel = getthatfuckingfresnel(0.04, data.normal, normalize(data.cameraPos), data.roughness);
    float x = 1.0 - max(0, dot(lightDir, data.normal));
    vec3 radiance = shade(CameraPosition, vec3(fresnel), data.normal, data.worldPos, data.worldPos - lightDir * 1.0, color * data.diffuseColor, max(0.004, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - data.roughness);    

    vec3 difradiance = shadeDiffuse(CameraPosition, data.diffuseColor * (1.0 - fresnel), data.normal, data.worldPos, data.worldPos - lightDir * 1.0, color, 0.0, false) * x;
    return radiance + difradiance; 
}


vec3 getDiffuseAtmosphereColor(){
    return textureLod(atmScattTex, SunDirection, textureQueryLevels(atmScattTex) - 3).rgb;
}

vec3 getSunColor(float roughness){
    vec3 atm = textureLod(atmScattTex, SunDirection, roughnessToMipmap(roughness, atmScattTex)).rgb * 1.0;
    return mix(atm * max(0.0, SunDirection.y) + vec3(1.0) , vec3(1.0), pow(max(0.01, SunDirection.y), 8.0));
}

vec3 getAtmosphereScattering(vec3 dir, float roughness){
    return textureLod(atmScattTex, dir, roughnessToMipmap(roughness, atmScattTex)).rgb;
}


vec3 shadeColor(PostProceessingData data, vec3 lightdir, vec3 c){
    return mix(shadingNonMetalic(data, lightdir, c), shadingMetalic(data, lightdir, c), data.metalness);// * (UseAO == 1 ? texture(aoxTex, UV).r : 1.0);
}

vec3 sampleAtmosphere(vec3 dir, float roughness, float sun){
    vec3 diffuse = getDiffuseAtmosphereColor()  ;
    vec3 direct = getSunColor(roughness);
    vec3 scattering = getAtmosphereScattering(dir, roughness);
    scattering += sun * (1.0 - step(0.1, length(currentData.normal))) * smoothstep(0.9987, 0.999, dot(SunDirection, dir)) * getSunColor(0.0) * 123.0;
    vec4 cloudsData = smartblur(dir, roughness);
    float coverage = cloudsData.r;
    float dist = cloudsData.g;
    float shadow = cloudsData.b;
    float rays = cloudsData.a;
    
    vec3 cloud = mix(diffuse, direct, shadow);
    return mix(scattering, cloud, coverage) + rays * direct;
}

vec2 projectvdao(vec3 pos){
    vec4 tmp = (VPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}

vec4 getLighting(){

    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);

    float hitdist = textureLod(waterDistanceTex, UV, 0.0).r;
    if(hitdist < 0.0) return  textureLod(inputTex, UV, 0.0);
    vec3 hitpos = CameraPosition + reconstructCameraSpaceDistance(UV, hitdist);
   // return hitdist * vec4(0.01);
    
    vec2 px = 1.0 / Resolution;
    mipmap1 = textureQueryLod(waterTileTex, hitpos.xz * WaterScale *  octavescale1).x * 0.6;
    float roughness = clamp((pow(mipmap1 / textureQueryLevels(waterTileTex), 1.0)) * WaterWavesScale, 0.0, 0.5) * 0.3;
    
    vec3 normal = normalx(hitpos, 0.03, roughness);
    
    vec3 dr2 = reconstructCameraSpaceDistance(UV + vec2(px.x, 0.0), 1.0);
    vec3 dr3 = reconstructCameraSpaceDistance(UV + vec2(0.0, px.y), 1.0);
    
    int smoothsteps = 2;
    float smoothstepsinv = 1.0 / float(smoothsteps);
    
    vec3 dfX = ((CameraPosition + dr2 * hitdist) - hitpos) * smoothstepsinv;
    vec3 dfY = ((CameraPosition + dr3 * hitdist) - hitpos) * smoothstepsinv;

    float smoothedgemult = smoothstep(0.0, 1.1, currentData.cameraDistance - hitdist);

    
    //float roughness = 0.0;
   // roughness = clamp((pow(textureQueryLod(waterTileTex, hitpos.xz * vec2(NoiseOctave2, NoiseOctave3) * octavescale1).x / textureQueryLevels(waterTileTex), 1.0)) * WaterWavesScale, 0.0, 1.0);
   /* if(WaterWavesScale > 0.01){
        float nw = 1.0;
        for(int x=-smoothsteps; x <= smoothsteps; x++){
            for(int y=-smoothsteps; y <= smoothsteps; y++){
                if(y == 0 && x == 0) continue;
                normal += normalx(hitpos + dfX * float(x) + dfY * float(y), 0.1);
                nw += 1.0;
            }
        }
        vec3 X = normal / nw;
       // roughness = clamp(length(dfX) + length(dfY), 0.0, 1.0);
        roughness = 1.0 - min(1.0, length(X) * length(X));
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
    //roughness = 1.0;
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
                    
    //result += mix(shadingWater(dataReflection, -SunDirection, getSunColor(0.0), sampleAtmosphere(dir, roughness, 0.0)) * fresnel * 1.0, getSunColor(0.5) * 0.33 * whites, min(1.0, whites));// + sampleAtmosphere(normal, 0.0) * fresnel;
    result += shadingWater(dataReflection, -SunDirection, getSunColor(0.0), sampleAtmosphere(dir, roughness, 0.0)) * fresnel * 1.0 * mix(1.0, 0.1, roughness);
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