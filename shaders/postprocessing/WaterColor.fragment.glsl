#version 430 core

layout(binding = 21) uniform sampler2D waterDistanceTex;
layout(binding = 22) uniform sampler2D inputTex;
layout(binding = 23) uniform sampler2D waterTileTex;

layout(binding = 25) uniform samplerCube coverageDistTex;
layout(binding = 26) uniform samplerCube shadowsTex;
layout(binding = 27) uniform samplerCube skyfogTex;
layout(binding = 24) uniform sampler2D starsTex;
layout(binding = 28) uniform sampler2D moonTex;
layout(binding = 29) uniform samplerCube resolvedAtmosphereTex;

#include PostProcessEffectBase.glsl
#include Atmosphere.glsl
#include CSM.glsl
uniform float WaterSpeed;
#include WaterHeight.glsl


float fogatt(float dist){
    dist *= 0.000015;
    return min(1.0, (dist * dist) );
}

float MIP = 0.0;
vec3 normalx(vec3 pos, float e, float roughness){
    vec2 ex = vec2(e, 0);
    vec3 a = vec3(pos.x, heightwaterD(pos.xz, MIP) * waterdepth, pos.z);
    vec3 b = vec3(pos.x - e, heightwaterD(pos.xz - ex.xy, MIP) * waterdepth, pos.z);
    vec3 c = vec3(pos.x, heightwaterD(pos.xz + ex.yx, MIP) * waterdepth, pos.z + e);
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
float hashX( float n ){
    return fract(sin(n)*758.5453);
}

float noise2X( in vec2 x ){
    vec2 p = floor(x);
    vec2 f = fract(x);
    float n = p.x + p.y*57.0;
    float res =
        mix (
            mix (
                hashX (
                    n + 0.0
                ), hashX (
                    n + 1.0
                ), f.x
            ), mix (
                hashX (
                    n + 57.0
                ), hashX (
                    n + 58.0
                ), f.x
            ), f.y
        );
    return res;
}

float roughnessToMipmapX(float roughness, samplerCube txt){
    //roughness = roughness * roughness;
    float levels = max(0, float(textureQueryLevels(txt)));
    float mx = log2(roughness*1024+1)/log2(1024);
    return mx * levels;
}
float rdhashx = 0.453451;
vec3 randpoint3x(){
    float x = hash(rdhashx);
    rdhashx += 2.1231255;
    float y = hash(rdhashx);
    rdhashx += 1.6271255;
    float z = hash(rdhashx);
    rdhashx += 1.1231255;
    return vec3(x, y, z) * 2.0 - 1.0;
}
float hitdst = 0.0;
vec4 getLighting(){
    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);

    float hitdist = textureLod(waterDistanceTex, UV, 0.0).r;

   // return vec4(mod(hitdist, 11.0));
    hitdst = length(currentData.normal) < 0.01 ? 99999.0 : currentData.cameraDistance;
    //return  textureLod(inputTex, UV, 0.0);
    if(hitdist <= 0.01) return  textureLod(inputTex, UV, 0.0);
    hitdst = min(hitdist, hitdst);
    vec3 hitpos = CameraPosition + reconstructCameraSpaceDistance(UV, hitdist);

    float planethit = intersectPlane(CameraPosition, dir, vec3(0.0, waterdepth + WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
    float planethit2 = intersectPlane(CameraPosition, dir, vec3(0.0, WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));

    vec3 nearsurface = CameraPosition + reconstructCameraSpaceDistance(UV, planethit > 0.0 && planethit2 > 0.0 ? planethit : hitdist);
    vec2 px = 1.0 / Resolution;
    mipmap1 = textureQueryLod(waterTileTex, nearsurface.xz * WaterScale *  octavescale1).x;
    float mipmap2 = textureQueryLod(waterTileTex, nearsurface.xz * WaterScale *  octavescale1 * 11.2467).x;
    float mipmap3 = textureQueryLod(waterTileTex, nearsurface.xz * WaterScale *  octavescale1 * 151.2467).x ;
    float roughness2 = clamp((pow(mipmap1/ textureQueryLevels(waterTileTex), 1.0)) , 0.0, 13.0);
    float roughness = clamp(roughness2 , 0.0, 1.0);
    //return vec4(1) * roughness;
    MIP = 0.6 * roughness * textureQueryLevels(waterTileTex);
    //mipmap1 *= 0.0;
    float height1  = heightwater(hitpos.xz);
   // float foam  = foamwater(hitpos.xz, 0);
    float height2  = heightwaterD(hitpos.xz, 0.8 * textureQueryLevels(waterTileTex));


    vec3 origdir = dir;

    vec3 normal = normalx(hitpos, 0.0098 + roughness * 0.01, roughness * 0.3);
//    normal = mix(normal, VECTOR_UP, roughness * roughness);
   // return pow(max(0.0, dot(normal, dayData.sunDir)), 10.0) * vec4(1);
  //  return vec4(normal.xyzz * vec4(1,0.2,1,0));

  //  normal = normalize(normal + 	normalx(hitpos * 11.2467, 3.0953, roughness) * 0.3);
   // normal = normalize(normal + normalx(hitpos * 51.2467, 3.0953, roughness) * 0.2);
    mipmap1 = mipmap2;
    //normal = normalize(normal + (1.0 - roughness) * normalx(hitpos * 11.2467, 0.0953, roughness) * 0.2 * (1.0 - mipmap2/ textureQueryLevels(waterTileTex)));
    //return normal.xyzz;
    //normal = normalize(normal + normalx(hitpos * 151.2467, 3.0953, roughness) * 0.1 * (1.0 - mipmap3/ textureQueryLevels(waterTileTex)));
    dir = reflect(origdir, normal);
     dir.y = abs(dir.y);
  //  dir.y = max(0.05, dir.y);
    float fresnel  = fresnel_effect(vec3(0.02), roughness, max(0.0, dot(dir, normal))).x;
  //  return fresnel * vec4(1);
    //float x =  textureQueryLod(waterTileTex, hitpos.xz * WaterScale *  octavescale1).x / textureQueryLevels(waterTileTex);
    // float h = hitwater2(hitpos, dir) * (1.0 - x) + x;

      //  dir = mix(normalize(VECTOR_UP * 2.0 - dir), dir, h);
    /////    fresnel *= mix(0.9, 1.0, h);
   vec3 result = vec3(0.0);

   // roughness = clamp(roughness, 0.0, 1.0);
   // roughness = 1.0 - pow(1.0 - roughness, 2.0);
    vec3 worldpos = hitpos;
    float camdist = hitdist;
   // roughness = 0.91;
    //float whites = max(0.0, noise2d(hitpos.xz*0.1 + Time * 0.1) * 2.0 - 1.0) * noise2d(hitpos.xz*33.0  + Time * 0.3 + noise2d(hitpos.xz*36.0)) * pow(max(0.0, dot(Wind, normal)) * 2.0, 7.0);
  //  whites *= 45.0;
   // return vec4(whites );

    PostProcessingData dataReflection = PostProcessingData(
        vec3(1.0),
        normal,
        normalize(cross(
            dir,
            hitpos
        )).xyz,
        hitpos,
        origdir * camdist,
        camdist,
        roughness * 0.3  ,
        1.0
    );

    //result += mix(shadingWater(dataReflection, -dayData.sunDir, getSunColor(0.0), sampleAtmosphere(dir, roughness, 0.0)) * fresnel * 1.0, getSunColor(0.5) * 0.33 * whites, min(1.0, whites));// + sampleAtmosphere(normal, 0.0) * fresnel;
    vec3 atm = vec3(0.0);
    //for(int i=0;i<1;i++){
        atm += textureLod(resolvedAtmosphereTex, dir, roughnessToMipmapX(roughness * (0.02 * WaterWavesScale), resolvedAtmosphereTex)).rgb;
    //}
    //vec3 atm =  textureLod(resolvedAtmosphereTex, dir, roughnessToMipmapX(roughness * (0.02 * WaterWavesScale), resolvedAtmosphereTex)).rgb;// * (1.0 - roughness * 0.5) * (1.0 - roughness * 0.5);
    //atm *= 0.1;
   // return vec4(atm, 1.0);
  // vec2 ssr = traceReflection(hitpos, dir);
   //ssr.x = 1.0 - step(0.0, ssr.x);
	//float dst = traceReflection(hitpos, dir);
	//return vec4(dst);
    result +=   shadingWater(dataReflection, normal, -dayData.sunDir, getSunColor(0.0), atm)  * 1.0 * mix(1.0, 1.0, roughness);
    vec3 refr = normalize(refract(origdir, normal, 0.66));
    if(length(currentData.normal) < 0.01) currentData.cameraDistance = 299999.0;
    float hitdepth = currentData.cameraDistance - hitdist;
    vec3 newposrefracted = hitpos + refr * min(25.0, hitdepth);

    PostProcessingData dataRefracted = currentData;
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

    vec3 newpos = CameraPosition + dir * planethit;
    vec3 newpos2 = CameraPosition + dir * planethit2;

    float covercloud = smoothstep(0.198, 0.300, (CloudsThresholdLow + CloudsThresholdHigh) * 0.5);

    //float ssscoeff = pow(max(0, height1 - (waterdepth*0.5)) * WaterWavesScale * 0.05 * length(WaterScale) , 2.0)  * 0.5 + 0.5;
    vec3 waterSSScolor =  vec3(0.01, 0.33, 0.55)*  0.071 ;

    result += waterSSScolor * getSunColor(0) * (0.3 + height1) * waterdepth * 0.3 * covercloud * max(0.0, dot(dayData.sunDir, VECTOR_UP));
   // result += 1.0 - smoothstep(0.002, 0.003, ssscoeff2);
    //result += (pow(dot(normal,dayData.sunDir) * 0.4 + 0.6,80.0) * vec3(0.8,0.9,0.6) * 0.12) * getSunColor(0) * (1.0 - fresnel)  * 0.8069;
    vec3 refr2 = normalize(refr + vec3(0.0, 0.3, 0.0));
    float superscat = pow(max(0.0, dot(refr, dayData.sunDir)), 8.0) ;//* (1.0 - fresnel);
    result += vec3(0.5,0.9,0.8) * superscat * getSunColor(0) * 8.0 * covercloud;
    return  vec4(result, 0.0);
}


vec4 shade(){
    vec4 color = getLighting();
    color.a = hitdst;
    return vec4(color);
}
