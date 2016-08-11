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

float rdhash = 0.453451 + Time;
vec2 randpoint2(){
    float x = rand2s(UV * rdhash);
    rdhash += 2.1231255;
    float y = rand2s(UV * rdhash);
    rdhash += 1.6271255;
    return vec2(x, y) * 2.0 - 1.0;
}
vec3 randpoint3(){
    float x = rand2s(UV * rdhash);
    rdhash += 2.1231255;
    float y = rand2s(UV * rdhash);
    rdhash += 1.6271255;
    float z = rand2s(UV * rdhash);
    rdhash += 1.6271255;
    return vec3(x, y, z) * 2.0 - 1.0;
}

float aaoo(){
    vec2 uv = reverseViewDir();
    float center = texture(cloudsCloudsTex, uv).b;
    float aoc = 0;
    for(int i=0;i<32;i++){
        vec2 rdp = randpoint2() * 0.01;
        float there = texture(cloudsCloudsTex, uv + rdp).b;
        float w = 1.0 - smoothstep(1000.0, 10000.0, center - there);
        aoc += w * clamp(center - there, 0.0, 1000.0) * 0.001;
    }
    return pow(1.0 - aoc / 32.0, 16.0);
}    
float roughtomipmap(float roughness, sampler2D txt){
    //roughness = roughness * roughness;
    float levels = max(0, float(textureQueryLevels(txt)));
    float mx = log2(roughness*512+1)/log2(512);
    return mx * levels;
}
vec4 smartblur(vec3 dir, float roughness){
    vec2 uv = reverseDir(dir);
    float levels = max(0, float(textureQueryLevels(cloudsCloudsTex)) - 2.0);
    float mx = log2(roughness*512+1)/log2(512);
    float mlvel = mx * levels;
    return textureLod(cloudsCloudsTex, uv, mlvel).rgba;
    vec4 centervals = textureLod(cloudsCloudsTex, uv, mlvel).rgba;
    vec4 centerval = vec4(0);
    float center = textureLod(cloudsCloudsTex, uv, mlvel).r;
    float aoc = 0;
    float blurrange = 0.003;// * pow(center, 2.0);
    for(int i=0;i<64;i++){
        vec2 rdp = randpoint2() * blurrange;
        float there = textureLod(cloudsCloudsTex, uv + rdp, mlvel).r;
        float w = 1.0;//pow( 1.0 - abs(there - center), 132.0);
        centerval += w * textureLod(cloudsCloudsTex, uv + rdp, mlvel).rgba;
        aoc += w;
    }
    centerval /= aoc;
    return vec4(centervals.r, centerval.g, 0.0, centerval.a);
}



float fogatt(float dist){
    dist *= 0.000015;
    return min(1.0, (dist * dist) );
}

#define wassnoise(a) pow((snoise(a) * 0.5 + 0.5), 1.0)
vec3 octaveN(vec2 a, float esp){
    vec2 zxpos = a * esp;
    float h1 = snoise(vec3(zxpos.xy, Time * 0.1));
    float h2 = wassnoise(vec3(zxpos.xy, Time * 0.1));
    float h3 = snoise(vec3(zxpos.xy, Time * 0.1));
    return normalize(vec3(h1, h2, h3));
}

float hashX( float n ){
    return fract(sin(n)*758.5453);
}

float noiseX( in vec3 x ){
    vec3 p = floor(x);
    vec3 f = fract(x); 
    float n = p.x + p.y*57.0 + p.z*800.0;
    float res = mix(mix(mix( hashX(n+  0.0), hashX(n+  1.0),f.x), mix( hashX(n+ 57.0), hashX(n+ 58.0),f.x),f.y),
    mix(mix( hashX(n+800.0), hashX(n+801.0),f.x), mix( hashX(n+857.0), hashX(n+858.0),f.x),f.y),f.z);
    return res;
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
#define ssin(a) (smoothstep(0.0,3.1415,a*3.1415) * 2.0 - 1.0) 
#define snoisesin(a) pow(1.0 - (abs(noise2X(a) - 0.5) * 2.0), 6.0)
#define snoisesinpow(a,b) pow(1.0 - (abs(noise2X(a) - 0.5) * 2.0), b)
#define snoisesinpowXF(a,b) (1.0 - pow((abs(snoise(a))), b))
float heightwater(vec2 pos, int s){
    pos *= 0.009;
    pos *= vec2(NoiseOctave2, NoiseOctave3);
    float res = 0.0;
    float w = 0.0;
    float wz = 1.0;
    float chop = 6.0;
    float tmod = 60.1;
    for(int i=0;i<s;i++){
        vec2 t = vec2(0, tmod * T001);
        res += wz * snoisesinpow(pos + t.yx, chop);
        res += wz * snoisesinpow(pos - t.yx, chop);
        w += wz * 2;
        wz *= 0.4;
        pos *= 2.4;
        tmod *= 1.8;
    }
    return res / w;// * 0.5 + (sin((noise2X(pos.xx * 0.001) * 0.5 + 0.5) * pos.x*0.01 + Time) * 0.5 + 0.5);
}
vec3 hitpos = vec3(0);
float hitdistx = 0;
#define waterdepth 22.0 * WaterWavesScale
#define WATER_SAMPLES_LOW 2
#define WATER_SAMPLES_HIGH 7
vec3 normalx(vec3 pos, float e){
    vec2 ex = vec2(e, 0);
    vec3 a = vec3(pos.x, heightwater(pos.xz, WATER_SAMPLES_HIGH) * waterdepth, pos.z);    
    vec3 b = vec3(pos.x + e, heightwater(pos.xz + ex.xy, WATER_SAMPLES_HIGH) * waterdepth, pos.z);
    vec3 c = vec3(pos.x, heightwater(pos.xz - ex.yx, WATER_SAMPLES_HIGH) * waterdepth, pos.z - e);      
    vec3 normal = (cross(normalize(a-b), normalize(a-c)));
    hitpos = pos;
    hitdistx = distance(CameraPosition, pos);
    return normalize(normal).xyz;// + 0.1 * vec3(snoise(normal), snoise(-normal), snoise(normal.zyx)));
}
vec3 normalxlow(vec3 pos, float e){
    vec2 ex = vec2(e, 0);
    vec3 a = vec3(pos.x, heightwater(pos.xz, WATER_SAMPLES_LOW) * waterdepth, pos.z);    
    vec3 b = vec3(pos.x + e, heightwater(pos.xz + ex.xy, WATER_SAMPLES_LOW) * waterdepth, pos.z);
    vec3 c = vec3(pos.x, heightwater(pos.xz - ex.yx, WATER_SAMPLES_LOW) * waterdepth, pos.z - e);      
    vec3 normal = (cross(normalize(a-b), normalize(a-c))); ;
    return normalize(normal).xyz;// + 0.1 * vec3(snoise(normal), snoise(-normal), snoise(normal.zyx)));
}

vec3 raymarchwaterImpl(vec3 upper, vec3 lower, float stepsF, int stepsI, float instepsF, int instepsI){
    float stepsize = 1.0 / stepsF;
    float iter = 0;
    float maxdist = length(currentData.normal) < 0.07 ? 999998.0 : length(currentData.cameraPos);
    for(int i=0;i<stepsI + 1;i++){
        vec3 pos = mix(upper, lower, iter);
      //  pos.x += iter * 30.0;
        float dst = distance(pos, CameraPosition);
        float h = heightwater(pos.xz, WATER_SAMPLES_LOW);
        if(h > 1.0 - (iter) || dst > maxdist) {
           // return normalx(pos, 1);
            iter -= stepsize;
            float stepsize = stepsize / instepsF;
            for(int z=0;z<instepsI + 1;z++){
                pos = mix(upper, lower, iter);
               // pos.x += (iter + rd) * 30.0;
                dst = distance(pos, CameraPosition);
                if(heightwater(pos.xz, WATER_SAMPLES_LOW) > 1.0 - iter || dst > maxdist) {
                    return normalx(pos, 0.01);
                }
                iter += stepsize;
            }
            return normalx(pos, 0.01);
        }
        iter += stepsize;
    }
    return normalx(upper, 0.01);
}
vec3 raymarchwater(vec3 upper, vec3 lower, int si, int isi){
    return raymarchwaterImpl(upper, lower, float(si), si, float(isi), isi);
}

vec3 raymarchwaterLOW3(vec3 upper, vec3 lower){
    vec3 n = normalx(upper, 1);
    hitpos = lower + vec3(0,heightwater(upper.xz, WATER_SAMPLES_HIGH), 0);
    hitdistx = distance(CameraPosition, hitpos);
    return n;
}

#define LOD3 4422.0

vec2 projectvdao(vec3 pos){
    vec4 tmp = (VPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}
vec3 cloudsbydir(vec3 dir){
    float fresnel = 1.0;
    float dst = 0;
    float roughness = 0.0;
    vec3 basewaterclor = vec3(0);
    vec3 defres = texture(directTex, UV).rgb + texture(alTex, UV).rgb * (UseAO == 1 ? texture(aoxTex, UV).r : 1.0);
    float whites = (0);
    float doatmscatter = 1.0;
   //defres += getAtmosphereForDirection(currentData.worldPos, currentData.normal, normalize(SunDirection), currentData.roughness) * 0.5 * currentData.diffuseColor;
    if(dir.y < 0.0){
        
        vec3 atmorg = vec3(0, planetradius, 0) + CameraPosition;  
        Ray r = Ray(atmorg, dir);
        float planethit = intersectPlane(CameraPosition, dir, vec3(0, waterdepth + NoiseOctave4*20.0, 0), vec3(0,1,0));
        float planethit2 = intersectPlane(CameraPosition, dir, vec3(0, 0.01 + NoiseOctave4*20.0, 0), vec3(0,1,0));    
        vec3 newpos = CameraPosition + dir * planethit;
        vec3 newpos2 = CameraPosition + dir * planethit2;
        float flh1 = planethit * 0.0005;
        roughness = 1.0 - smoothstep(0.0, 22.0, sqrt(sqrt(flh1)));
        roughness = 1.0 - pow(roughness, 164.0);
        roughness = mix(roughness, 1.0, 1.0 - pow(abs(dir.y), 1.0));
        dst = planethit;
        float lodz = pow(1.0 - smoothstep(0.0, LOD3, planethit), 4.0);
        vec3 n = vec3(0,1,0);
        float waveheight = 0.0;
        if(WaterWavesScale > 0.0){
            if(planethit >= LOD3) {
                n = raymarchwaterLOW3(newpos, newpos2);
            }
            else {
                n = mix(raymarchwaterLOW3(newpos, newpos2), raymarchwater(newpos, newpos2, int(2.0 + 6.0 * lodz * WaterWavesScale), int(2.0 + 8.0 * lodz * WaterWavesScale)), 1.0 - smoothstep(0.0, LOD3, planethit));
            }
            float freq = 1.0;
            float h0 = heightwater(hitpos.xz, WATER_SAMPLES_HIGH-3);
            waveheight = h0;
            float h1 = heightwater(hitpos.xz + vec2(freq, 0.0), WATER_SAMPLES_HIGH-3);
            float h2 = heightwater(hitpos.xz + vec2(0.0, freq), WATER_SAMPLES_HIGH-3);
            float h3 = heightwater(hitpos.xz - vec2(freq, 0.0), WATER_SAMPLES_HIGH-3);
            float h4 = heightwater(hitpos.xz - vec2(0.0, freq), WATER_SAMPLES_HIGH-3);
            whites = max(0, h0 - (h1 + h2 + h3 + h4)*0.25) * 122.0 * pow(h0, 3.0);
            whites = min(1.0, pow(whites, 3.0));
            whites *= (1.0 - smoothstep(0.0, LOD3, planethit)) * WaterWavesScale;
            //return vec3(whites);
            float lodzx =  1.0 - clamp(hitdistx / LOD3, 0.0, 1.0);
            //roughness = roughness * 0.8 + 0.2;
            n = normalize(mix(n, vec3(0,1,0), roughness));
        }
        //return vec3(whites);
        roughness *= 0.1 * (WaterWavesScale);
        vec3 refr = normalize(refract(dir, n, 1.333));
        dir = normalize(reflect(dir, n));
        float hitdepth = currentData.cameraDistance - hitdistx;
        vec3 newposr = currentData.worldPos + refr * hitdepth;
        fresnel = fresnel_again(vec3(0.00), n, dir, 1.0);
        //return vec3(fresnel);
        vec2 uv = UV;
        if(hitdistx > 0 && hitdistx < currentData.cameraDistance) uv = projectvdao(newposr);
        defres = texture(directTex, uv).rgb + texture(alTex, uv).rgb * (UseAO == 1 ? texture(aoxTex, uv).r : 1.0);
        //defres += getAtmosphereForDirection(currentData.worldPos, n, normalize(SunDirection), currentData.roughness) * 0.5 * currentData.diffuseColor;
        basewaterclor = (1.0 - fresnel) * mix(vec3(0.0, 0.46, 0.60) * max(0, normalize(SunDirection).y)* 0.1 * (waveheight * 0.3 + 0.7), defres, (1.0 / (hitdepth * 0.08 + 1.0)));
      //  dir = normalize(mix(dir, vec3(0,1,0), roughness));
      //return vec3(1) * (1.0 / (hitdepth * hitdepth * 0.03 + 1.0));
        //dir = normalize(reflect(dir, vec3(0,1,0)));
        doatmscatter = 0;
    } else {
        roughness = 0;//pow(1.0 - (dir.y), 128.0);
    }
   // roughness = 0;
    //return vec3(cdata.a);
    vec4 cdata = smartblur(dir, roughness).rgba;
    cdata.a = doatmscatter * clamp(pow(cdata.a * 1.0, 16.0) * 1.0, 0.0, 1.0) * NoiseOctave1;
    cdata.r *= 1.0 - pow(1.0 - (dir.y), 64.0);
    vec3 scatt = getAtmosphereForDirectionReal(vec3(0,1,0), (dir), normalize(SunDirection)) + sun(dir, normalize(SunDirection), dir.y < 0.0 ? 1.0 : (1.0 - roughness) );
    vec3 skydaylightcolor = vec3(0.23, 0.33, 0.48) * 1.3;
    vec3 atmcolor = getAtmosphereForDirection(vec3(0), normalize(SunDirection), normalize(SunDirection), 0.2) + vec3(1);
    vec3 atmcolor1 = getAtmosphereForDirection(vec3(0), vec3(0,1,0), normalize(SunDirection), 0.0);
        float diminisher = max(0, dot(normalize(SunDirection), vec3(0,1,0)));
    float diminisher_absolute = dot(normalize(SunDirection), vec3(0,1,0)) * 0.5 + 0.5;
    
    float dimpow = 1.0 -  diminisher;
    float dmxp = max(0.01, pow(1.0 - max(0, -normalize(SunDirection).y), 32.0));
    

    vec3 shadowcolor = mix(skydaylightcolor, skydaylightcolor * 0.05, 1.0 - diminisher);
    vec3 litcolor = mix(vec3(10.0)   , atmcolor * 0.2, 1.0 - diminisher);
    vec3 colorcloud = dmxp * mix(shadowcolor, litcolor, pow(cdata.g, 2.0)) ;//* (diminisher * 0.3 + 0.7);
    
    //cdata.r = mix(cdata.r, 0.0, fogatt(cdata.b));
    //   cdata.r = mix(cdata.r, 0.0, min(1.0, dst * 0.000005));
    vec3 scatcolor = mix(vec3(1.0), atmcolor, 1.0 - diminisher) * 0.1;
    cdata.a = mix(1.0, cdata.a, 1.0 - pow(1.0 - dir.y, 24.0));
    vec3 result = vec3(litcolor) * whites + fresnel * mix(scatt, colorcloud, min(1.0, cdata.r * 1.2)) + basewaterclor;
    result = clamp(result, vec3(0.0), vec3(30.0));
    cdata.a *= pow(dot(dir, normalize(SunDirection))*0.5 + 0.5, 3.0);
    result += cdata.a * atmcolor;// + diminisher_absolute * (0.5 * pow(diminisher, 8.0) + 0.5) * litcolor * ((pow(1.0 - diminisher, 24.0)) * 0.9 + 0.1) * pow(cdata.a * 1.0, 2.0);
    
    //return vec3(hitdistx);
  //  return result;
   // return texture(atmScattTex, UV).rgb;
   if(hitdistx > 0 && hitdistx < currentData.cameraDistance || length(currentData.normal) < 0.01) return result;
   else return defres;//mix(result, vec3(0.7), ;
   //return texture(atmScattTex, UV).rgb;
  // return texture(atmScattTex, UV).rgb;
  // eturn getatscatter(dir, normalize(SunDirection))+ sun(dir, normalize(SunDirection));
   //    return vec3(1)  * cdata.g;
}

vec3 fisheye(){
    vec2 fullsp = UV * 2.0 - 1.0;
    //vec3 dir = normalize(reconstructCameraSpaceDistance(UV * 0.5 + 0.5, 1.0));
  //  fullsp = fullsp / sqrt(1.0 - length(fullsp) * 0.71);
   // fullsp *= 1.5;
    vec3 rld = normalize(reconstructCameraSpaceDistance(vec2(0.5), 1.0));
    vec3 dir = normalize(reconstructCameraSpaceDistance(fullsp * 0.5 + 0.5, 1.0));
    vec3 xdir = rld - dir;
   // xdir.y *= 0.15;
    dir -= xdir * 0.8;
   // dir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    //vec3 dir = normalize(reconstructCameraSpaceDistance((fullsp * 3.0) * 0.5 + 0.5, 1.0));
    return normalize(dir);
}

vec4 shade(){    
    vec3 color = cloudsbydir(fisheye());
    return vec4( color, 1.0);
}