#version 430 core


layout(binding = 3) uniform samplerCube skyboxTex;
layout(binding = 5) uniform sampler2D directTex;
layout(binding = 6) uniform sampler2D alTex;
layout(binding = 16) uniform sampler2D aoxTex;
layout(binding = 20) uniform sampler2D fogTex;

layout(binding = 25) uniform samplerCube coverageDistTex;
layout(binding = 26) uniform samplerCube shadowsTex;
layout(binding = 27) uniform samplerCube skyfogTex;
layout(binding = 23) uniform sampler2D waterTileTex;

uniform int UseAO;
uniform float T100;
uniform float T001;


#include PostProcessEffectBase.glsl
#include Atmosphere.glsl
#include noise2D.glsl
#include CSM.glsl

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
/*
float aaoo(){
    vec2 uv = reverseViewDir();
    float center = texture(cloudsCloudsTex, uv).b;
    float aoc = 0;
    for(int i=0;i<32;i++){
        vec2 rdp = randpoint2() * 0.01;
        float there = texture(cloudsCloudsTex, uv + rdp).b;
        float w = 1.0 - smoothstep(1000.0, 10000, center - there);
        aoc += w * clamp(center - there, 0.0, 1000.0) * 0.001;
    }
    return pow(1.0 - aoc / 32.0, 16.0);
}    */
float roughtomipmap(float roughness, sampler2D txt){
    //roughness = roughness * roughness;
    float levels = max(0, float(textureQueryLevels(txt)));
    float mx = log2(roughness*512+1)/log2(512);
    return mx * levels;
}

float blurshadows(vec3 dir, float roughness){
    if(CloudsDensityScale <= 0.010) return 0.0;
    float levels = max(0, float(textureQueryLevels(shadowsTex)));
    float mx = log2(roughness*1024+1)/log2(1024);
    float mlvel = mx * levels;
    float dst = textureLod(cloudsCloudsTex, dir, mlvel).g;
    float aoc = 1.0;
    float centerval = textureLod(shadowsTex, dir, mlvel).r;
    float blurrange = 0.01;
    for(int i=0;i<7;i++){
        vec3 rdp = normalize(dir + randpoint3() * blurrange);
        float there = textureLod(cloudsCloudsTex, rdp, mlvel).g;
        float w = clamp(1.0 / (abs(there - dst)*0.01 + 0.01), 0.0, 1.0);
        centerval += w * textureLod(shadowsTex, rdp, mlvel).r;
        aoc += w;
    }
    centerval /= aoc;
    return centerval;
}
float blurskyfog(vec3 dir, float roughness){
    if(NoiseOctave1 <= 0.010) return 0.0;
    float levels = max(0, float(textureQueryLevels(skyfogTex)));
    float mx = log2(roughness*128+1)/log2(128);
    float mlvel = mx * levels;
    float dst = textureLod(cloudsCloudsTex, dir, mlvel).g;
    float aoc = 1.0;
    float centerval = textureLod(skyfogTex, dir, mlvel).r;
    float blurrange = 0.08;
    for(int i=0;i<7;i++){
        vec3 rdp = normalize(dir + randpoint3() * blurrange);
        float there = textureLod(cloudsCloudsTex, rdp, mlvel).g;
        float w = clamp(1.0 / (abs(there - dst)*0.01 + 0.01), 0.0, 1.0);
        centerval += w * textureLod(skyfogTex, rdp, mlvel).r;
        aoc += w;
    }
    centerval /= aoc;
    return centerval;
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
    //ret.w = blurskyfog(dir, roughness);
    return ret;
    // return textureLod(cloudsCloudsTex, dir, mlvel).rgba;
    vec4 centervals = textureLod(cloudsCloudsTex, dir, mlvel).rgba;
    vec4 centerval = vec4(0);
    vec2 center = textureLod(cloudsCloudsTex, dir, mlvel).rg;
    float aoc = 0;
    float blurrange = 0.001;
    for(int i=0;i<12;i++){
        vec3 rdp = normalize(dir + randpoint3() * blurrange);
        float there = textureLod(cloudsCloudsTex, rdp, mlvel).b;
        float w = pow( 1.0 - abs(there - centervals.b), 2.0);
        centerval += w* textureLod(cloudsCloudsTex, rdp, mlvel).rgba;
        aoc += w;
    }
    centerval /= aoc;
    return vec4(centerval.r, centerval.g, centerval.b, centerval.a);
}



float fogatt(float dist){
    dist *= 0.000015;
    return min(1.0, (dist * dist) );
}


float waterhitdistance = 1.0;
float angle = 2.3999;// god damn god angle
mat2 octave_rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
float octavescale1 = 0.018;
float mipmap1 = 0.0;
float heightwater(vec2 uv, int s){
    uv *= vec2(NoiseOctave2, NoiseOctave3) * octavescale1;
    return textureLod(waterTileTex, uv, mipmap1).r;
}


vec3 hitpos = vec3(0);
float hitdistx = 0;
#define waterdepth 1.0 * WaterWavesScale
#define WATER_SAMPLES_LOW 2
#define WATER_SAMPLES_HIGH 4
vec3 normalx(vec3 pos, float e){
    hitpos = pos;
    hitdistx = distance(CameraPosition, pos);
    vec2 ex = vec2(e, 0);
    vec3 a = vec3(pos.x, heightwater(pos.xz, WATER_SAMPLES_HIGH) * waterdepth, pos.z);    
    vec3 b = vec3(pos.x + e, heightwater(pos.xz + ex.xy, WATER_SAMPLES_HIGH) * waterdepth, pos.z);
    vec3 c = vec3(pos.x, heightwater(pos.xz - ex.yx, WATER_SAMPLES_HIGH) * waterdepth, pos.z - e);      
    vec3 normal = (cross(normalize(a-b), normalize(a-c)));
    return normalize(normal).xyz;// + 0.1 * vec3(snoise(normal), snoise(-normal), snoise(normal.zyx)));
}

vec3 raymarchwaterImpl(vec3 upper, vec3 lower, int stepsI){
    //return normalx(upper, 0.1);
    float stepsize = 1.0 / stepsI;
    float iter = 0;
    float rdo = rand2s(UV * Time);
    float rd = stepsize * rdo;
    float maxdist = length(currentData.normal) < 0.07 ? 999998.0 : currentData.cameraDistance;
    for(int i=0;i<stepsI + 1;i++){
        vec3 pos = mix(upper, lower, iter + rd);
        //  pos.x += iter * 30.0;
        float dst = distance(pos, CameraPosition);
        float h = heightwater(pos.xz, WATER_SAMPLES_LOW);
        if(h > 1.0 - (iter + rd) || dst > maxdist) {
            return normalx(pos, 0.1);
        }
        iter += stepsize;
    }
    return normalx(upper, 0.1);
}
vec3 raymarchwater(vec3 upper, vec3 lower, int si){
    return raymarchwaterImpl(upper, lower, si);
}


#define WaterLevel (0.01+ 10.0 * NoiseOctave6)


#define LOD3 422.0

vec3 atmc = getAtmosphereForDirection(normalize(SunDirection), currentData.roughness) * 55.0 * normalize(SunDirection).y;

float getthatfuckingfresnel(float reflectivity, vec3 normal, vec3 cameraSpace, float roughness){
    vec3 dir = normalize(reflect(normalize(cameraSpace), normal));
    float base =  1.0 - abs(dot(normalize(normal), dir));
    float fresnel = (reflectivity + (1-reflectivity)*(pow(base, mix(5.0, 0.8, roughness))));
    float angle = 1.0 - base;
    return fresnel * (1.0 - roughness);
}

vec3 shadingMetalic(PostProceessingData data){
    float fresnelR = getthatfuckingfresnel(data.diffuseColor.r, data.normal, data.cameraPos, data.roughness);
    float fresnelG = getthatfuckingfresnel(data.diffuseColor.g, data.normal, data.cameraPos, data.roughness);
    float fresnelB = getthatfuckingfresnel(data.diffuseColor.b, data.normal, data.cameraPos, data.roughness);
    float fresnel = getthatfuckingfresnel(0.04, data.normal, normalize(data.cameraPos), data.roughness);
    vec3 newBase = vec3(fresnelR, fresnelG, fresnelB);
    //   return vec3(fresnel);
    float x = 1.0 - max(0, -dot(normalize(SunDirection), currentData.originalNormal));
    return shade(CameraPosition, newBase, data.normal, data.worldPos, data.worldPos + normalize(SunDirection) * 40.0, vec3(atmc),  max(0.004, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - currentData.roughness);
}

vec3 shadingNonMetalic(PostProceessingData data){
    float fresnel = getthatfuckingfresnel(0.04, data.normal, normalize(data.cameraPos), data.roughness);
    float x = 1.0 - max(0, -dot(normalize(SunDirection), currentData.normal));
    
    vec3 radiance = shade(CameraPosition, vec3(fresnel), data.normal, data.worldPos, data.worldPos + normalize(SunDirection) * 40.0, vec3(atmc) * currentData.diffuseColor, max(0.004, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - currentData.roughness);    
    
    vec3 difradiance = shadeDiffuse(CameraPosition, data.diffuseColor * (1.0 - fresnel), data.normal, data.worldPos, data.worldPos + normalize(SunDirection) * 40.0, vec3(atmc), 0.0, false) * x;
    //   return vec3(0);
    return radiance + difradiance ; 
}

vec3 MakeShading(PostProceessingData data){
    return mix(shadingNonMetalic(data), shadingMetalic(data), data.metalness);// * (UseAO == 1 ? texture(aoxTex, UV).r : 1.0);
}
vec3 MakeShadingGI(PostProceessingData data){
    return shadeInvariantGI(CameraPosition, data.diffuseColor, data.worldPos, data.worldPos + normalize(SunDirection) * 40.0, vec3(atmc));
}
float causs(vec2 sp){
    vec2 p = sp*6.0 - vec2(125.0);
    vec2 i = p;
    float c = 1.0;
    float cc = fract(1.);
    int ccc = int(cc);
    
    float inten = 0.01;
    
    for (int n = 0; n < 4; n++) 
    {
        float t = Time * 0.2* (0.05 - (3.0 / float(n+1)));
        i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
        c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
    }
    c /= float(4);
    c = 1.5-sqrt(c);
    return c*c*c;
}

float snoisefractal(vec3 pos){
    return 0;
    pos *= (snoise(pos * 0.000005) * 0.5 + 0.5) * 0.2 + 0.8;
    float res = (snoise(pos * 0.000001) * 0.5 + 0.5) + 
    (snoise(pos * 0.00032) * 0.5 + 0.5) * 0.5 + 
    (snoise(pos * 0.000816 + snoise(pos * 0.000316) * 0.3) * 0.5 + 0.5) * 0.25 +
    (snoise(pos * 0.001816 + snoise(pos * 0.000516) * 0.3) * 0.5 + 0.5) * 0.125 +
    (snoise(pos * 0.003816 + snoise(pos * 0.001216) * 1.3) * 0.5 + 0.5) * 0.065;
    return smoothstep(0.88, 0.99, res);
}

// todo make it eye pos dependant 
float makeUpperLayer(vec3 dir){
   // return 0;
    Sphere upperlayer = Sphere(vec3(0), planetradius + 27000.0);
    vec3 atmorg = vec3(0, planetradius, 0) + CameraPosition;  
    Ray r = Ray(atmorg, dir);
    float ulhit = rsi2(r, upperlayer);
    vec3 p = CameraPosition + dir * ulhit;
    return snoisefractal(p*0.4);
}

vec2 projectvdao(vec3 pos){
    vec4 tmp = (VPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}

vec3 cloudsbydir(vec3 dir){
    bool underwater = CameraPosition.y < WaterLevel || WaterLevel + waterdepth * heightwater(CameraPosition.xz, WATER_SAMPLES_LOW) > CameraPosition.y;   
    //return textureLod(waterTileTex, UV * 0.1, 0.0 ).rrr;
    float planethit = intersectPlane(CameraPosition, dir, vec3(0.0, waterdepth + WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
    vec2 uvX = UV;
    if(underwater){
        float dst = planethit > 0.0 ? min(planethit, currentData.cameraDistance) : currentData.cameraDistance;
        dst = dst == 0.0 ? 99999.0 : dst;
        uvX = uvX + vec2(snoise2d(uvX * 3.0 + Time * 0.2), snoise2d(uvX.yx * 3.0 + Time * 0.2)) * min(0.006, dst * 0.0001);
        currentData = loadData(uvX);      
        dir = reconstructCameraSpaceDistance(uvX, 1.0);
        planethit = intersectPlane(CameraPosition, dir, vec3(0.0, waterdepth + WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
    }
    float dst = 0;
    float roughness = 0.0;
    vec3 basewaterclor = vec3(0);
    
    vec3 atmcolor = getAtmosphereForDirection(normalize(max(vec3(-1.0, 0.08, -1.0), SunDirection)), 0.0);
    
    float diminisher = max(0, 0.2 + 0.8 * dot(normalize(SunDirection), vec3(0,1,0)));
    float diminisherX = max(0,  dot(normalize(SunDirection), vec3(0,1,0)));
    vec3 litcolor2 = mix(vec3(8.0), atmcolor * 0.2, pow(1.0 - diminisher, 3.0));
    float cwpgi = 0;//CSMQueryVisibilityGI(currentData.worldPos) * 0.1 * diminisher * diminisher;
    float ln = length(currentData.normal);
    float fresnel = ln < 0.01 ? 1.0 : 0.0;
    float whites = (0);
    float doatmscatter = 1.0 ;
    //defres += getAtmosphereForDirection(currentData.worldPos, currentData.normal, normalize(SunDirection), currentData.roughness) * 0.5 * currentData.diffuseColor;
    Sphere planet0 = Sphere(vec3(0), planetradius + waterdepth + WaterLevel);
    vec3 atmorg = vec3(0, planetradius, 0) + CameraPosition;  
    Ray r = Ray(atmorg, dir);
    float shadowwater = 0.0;
    float hitdepth = 0.0;
    bool underwaterRoughness = false;
    float adf = 0.0;
    vec3 refr;
    float caustistics = 1.0;
    float ward = 0.0;
    if(intersects(planethit)){ 
        vec3 newpos = CameraPosition + dir * planethit;
        vec2 xymip = newpos.xz * vec2(NoiseOctave2, NoiseOctave3) * octavescale1;
        mipmap1 = 0.0;// * textureQueryLod(waterTileTex, xymip).x;
        Ray r = Ray(atmorg, dir);
        waterhitdistance = planethit;
        Sphere planet1 = Sphere(vec3(0), planetradius + WaterLevel);
        float planethit2 = intersectPlane(CameraPosition, dir, vec3(0.0, WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
        vec3 dfx = dFdx(newpos);
        vec3 dfy = dFdy(newpos);
        vec3 pixels = vec3(1.0 / Resolution.x, 1.0 / Resolution.y, 0.0);
        float dst = distance(CameraPosition, newpos);
        vec3 adfx = normalize(dir) * dst - reconstructCameraSpaceDistance(UV + pixels.xz, dst);
        vec3 adfy = normalize(dir) * dst - reconstructCameraSpaceDistance(UV + pixels.yz, dst);
        float df = length(dfx) + length(dfy);
        adf = length(adfx) + length(adfy);
        adf *= 0.1;
        float test = distance(CameraPosition, newpos) / length(Resolution.xy);
        //return (UV.x < 0.5 ? adf : df) * vec3(1);
        vec3 newpos2 = CameraPosition + dir * planethit2;

        //
        float flh1 = planethit * 0.001;
        roughness = smoothstep(0.0, 12.0, sqrt(sqrt(flh1)));
        dst = planethit;
        float lodz = pow(1.0 - smoothstep(0, LOD3, planethit), 1.0);
        vec3 n = vec3(0,1,0);
        float waveheight = 0.0;
        if(WaterWavesScale >= 0.0){
            if(planethit >= LOD3) {
                n = normalx(newpos, 0.1);
            }
            else {
                vec3 lowres = normalx(newpos, 0.1);
                float dotdir = 1.0 - pow( max(0.0, dot(normalize(newpos - CameraPosition), vec3(0.0, -1.0, 0.0))), 2.0);
                vec3 fullres = raymarchwater(newpos, newpos2, 1 + int(22.0 * WaterWavesScale * max(0.2, dotdir)));
                n = mix(lowres, fullres, pow(1.0 - smoothstep(0, LOD3, planethit), 2.0));
            }
            shadowwater = CSMQueryVisibilityVariable(hitpos, 6, 2.0);
        }
        vec3 newposW = CameraPosition + dir * hitdistx;
        vec3 origdir = dir;
        //return vec3(whites);
        roughness = dst * 0.0003 * WaterWavesScale;
        roughness = clamp(roughness, 0.0, 1.0);
        roughness = 1.0 - pow(1.0 - roughness, 2.0);
        dir = normalize(reflect(dir, n));
        dir.y = abs(dir.y);
        dir = mix(dir, reflect(origdir, vec3(0.0, 1.0, 0.0)), roughness);
        n = mix(n, vec3(0.0, 1.0, 0.0), roughness);
        refr = normalize(refract(origdir, n, 0.15));
        //return abs(refr);
        hitdepth = currentData.cameraDistance - hitdistx;
        vec3 newposr = hitpos + refr * hitdepth*200.0;
        fresnel = fresnel_again(vec3(0.04), n, dir, 1.0);
        if((hitdistx > 0 && hitdistx > currentData.cameraDistance) && ln > 0.01) {
            fresnel = 0.0;
        } else {
            underwaterRoughness = true;
        }
        doatmscatter = 0;
    } else {
        roughness = 0;
        shadowwater = 1.0;
    }
    
    if(underwaterRoughness) {
        hitdepth = currentData.cameraDistance - hitdistx;
        vec3 newposr = hitpos + refr * hitdepth*0.1;
        
        uvX =  abs(projectvdao(newposr));
        currentData = loadData(uvX);
        currentData.roughness = 1.0;
    }

    if(underwater || underwaterRoughness){
        float causshit = intersectPlane(currentData.worldPos, normalize(SunDirection), vec3(0.0, WaterLevel + waterdepth, 0.0), vec3(0.0, -1.0, 0.0));
        vec3 poscauss = currentData.worldPos + normalize(SunDirection) * causshit;
        caustistics = pow(mix(1.0, causs(poscauss.xz * 0.01), min(1.0, WaterWavesScale)), 3.0) * (1.0 / (1.0 + causshit / NoiseOctave7 * 0.1));
        //return caustistics * vec3(1);
    }


    float cwp = CSMQueryVisibility(currentData.worldPos) * (max(0, caustistics));
    // spaghetti incoming
    vec3 defres = ln < 0.01 ? vec3(0) : (texture(directTex, uvX).rgb + texture(alTex, uvX).rgb * (UseAO == 1 ? texture(aoxTex, uvX).r : 1.0) + MakeShading(currentData) * cwp + currentData.diffuseColor * 0.009 * litcolor2 * (UseAO == 1 ? texture(aoxTex, uvX).r : 1.0));
    
    vec3 mixing = vec3(
    1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(hitdepth) * pow(hitdepth, 1.4)),
    1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(hitdepth) * pow(hitdepth, 1.3)),
    1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(hitdepth) * pow(hitdepth, 1.2))
    );
    /*
    if(underwater) {
        float dst = planethit > 0 ? min(currentData.cameraDistance, planethit) : currentData.cameraDistance;
        mixing = vec3(
            1.0 - smoothstep(0.0, 33.0 * NoiseOctave7, sign(dst) * pow(dst, 1.4)),
            1.0 - smoothstep(0.0, 33.0 * NoiseOctave7, sign(dst) * pow(dst, 1.2)),
            1.0 - smoothstep(0.0, 33.0 * NoiseOctave7, dst)
        );
    }*/
    
    if(!underwater) defres = (1.0 - fresnel) * mix(vec3(0.0, 0.76, 0.55) * 0.1 * max(0, normalize(SunDirection).y * 0.9), defres, (ln > 0.01) ? (mixing) : vec3(0.0));
    currentData = loadData(UV);

    vec4 cdata = smartblur(dir, roughness).rbga;
    //if(UV.x < 0.5) return cdata.bbb * 0.00004;
    //return cdata.aaa;
    // return pow(vec3(cdata.b) * 0.0003, vec3(2.0));
    // return cdata.ggg;
    //cdata.a = doatmscatter ;//* clamp(pow(cdata.a * 1.0, 16.0) * 1.0, 0.0, 1.0);
    vec3 skydaylightcolor = vec3(0.23, 0.33, 0.48) * 1.3;
    vec3 sunx = sun(dir, normalize(SunDirection), 1.0 - roughness,  roughness * 0.6) * vec3(atmcolor) * 40.0 * shadowwater;
   
    vec3 scatt = getAtmosphereForDirection(dir, roughness * 0.1) + sunx;
    vec3 aahaha = textureLod(atmScattTex, vec3(0, 1, 0), textureQueryLevels(atmScattTex) - 1).rgb;
    vec3 atmcolor1 = getAtmosphereForDirection(vec3(0,1,0), 0.0);
    float dirdirdir = pow(16.0, max(0, dot(normalize(SunDirection), normalize(dir))));
    float diminisher_absolute = dot(normalize(SunDirection), vec3(0,1,0)) * 0.5 + 0.5;
    
    float dimpow = 1.0 -  diminisher;
    float dmxp = max(0.01, pow(1.0 - max(0, -normalize(SunDirection).y), 32.0));
    

    vec3 shadowcolor = mix(aahaha, aahaha, 1.0 - diminisherX);
    vec3 scatcolor = mix(vec3(1.0), atmcolor, 1.0 - diminisher) ;
    vec3 litcolor = mix(vec3(7.0) + dirdirdir * 1.2   , atmcolor * 0.2, pow(1.0 - diminisherX, 7.0));
    vec3 colorcloud = dmxp * mix(aahaha* (1.0 / (1.0 + MieScattCoeff * 0.4)), litcolor* (1.0 / (1.0 + MieScattCoeff * 0.4)), cdata.g)  ;//* (diminisher * 0.3 + 0.7);
    
    float xv  = 0.0;
    
    vec3 scatcolorsky = cdata.a * scatcolor;
    //return cdata.aaa;
    vec3 result = mix(scatt, colorcloud, min(1.0, makeUpperLayer(dir))); 
    result = mix(result, colorcloud*0.8, min(1.0, cdata.r * 1.2));
    result = result * fresnel + fresnel * cdata.a * scatcolor;
    result = clamp(result * (1.0 / (NoiseOctave1 + 1.0)) + scatcolorsky, vec3(0.0), vec3(300.0));
    
    
    if(currentData.cameraDistance == 0) currentData.cameraDistance = min(99999.0, hitdistx);
    float csdr = currentData.cameraDistance;
    if(hitdistx > 0)currentData.cameraDistance = min(currentData.cameraDistance, hitdistx);
    if(cdata.b > 0)currentData.cameraDistance = min(currentData.cameraDistance, cdata.b);
    float minshadow = shadowwater * 0.3 + 0.7;
    //return csdr * vec3(0.001);
    if(!underwater){
        if( ln < 0.01) 
        result = result * minshadow + defres * minshadow;
        else if(hitdistx > 0 && hitdistx > csdr && ln > 0.01) 
        result = defres;
        else if(hitdistx > 0 && hitdistx < csdr && ln > 0.01){
            result = result * minshadow + defres * minshadow;
        } else result = defres;//mix(result, vec3(0.7), ;
        //result = mix(result, (ln < 0.01 && hitdistx <= 0 ? result : vec3(0)) + litcolor * 0.08 * textureLod(fogTex, UV, 0).r, 1.0 - clamp(textureLod(fogTex, UV, 0).g, 0.0, 1.0));
        result = mix(result,   textureLod(fogTex, UV, 0).rrr * litcolor * 0.08, 1.0 - clamp(textureLod(fogTex, UV, 0).g, 0.0, 1.0));
    } else {
        //if(underwater) {
        float dst = csdr;
        dst = dst == 0.0 ? 99999.0 : dst;
        mixing = vec3(
        1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(dst) * pow(dst, 1.4)),
        1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(dst) * pow(dst, 1.3)),
        1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(dst) * pow(dst, 1.2))
        );
        //} 
        //return csdr * vec3(0.001);
        // return defres + result;
        return mix(vec3(0.0, 0.76, 0.55) * 0.1 * max(0, normalize(SunDirection).y * 0.9), defres + result, mixing);
        //return defres + result;
        if( ln < 0.01) 
        result = result * minshadow + defres * minshadow;
        else if(hitdistx > 0 && hitdistx > csdr && ln > 0.01) 
        result = defres;
        else if(hitdistx > 0 && hitdistx < csdr && ln > 0.01){
            result = result * minshadow + defres * minshadow;
        } else result = defres;//mix(result, vec3(0.7), ;
        result = mix(result, (ln < 0.01 && hitdistx <= 0 ? result : vec3(0)) + litcolor * 0.08 * textureLod(fogTex, UV, 0).r, 1.0 - clamp(textureLod(fogTex, UV, 0).g, 0.0, 1.0));   
    }
    if(currentData.cameraDistance == 0) currentData.cameraDistance = cdata.b;
    return result;
}

vec4 shade(){    
    vec3 color = cloudsbydir(reconstructCameraSpaceDistance(UV, 1.0));
    return vec4( clamp(color, 0.0, 10.0), currentData.cameraDistance * 0.001);
}