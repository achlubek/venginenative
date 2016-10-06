#version 430 core


layout(binding = 3) uniform samplerCube skyboxTex;
layout(binding = 5) uniform sampler2D directTex;
layout(binding = 6) uniform sampler2D alTex;
layout(binding = 16) uniform sampler2D aoxTex;
layout(binding = 20) uniform sampler2D fogTex;
layout(binding = 21) uniform sampler2D waterColorTex;
layout(binding = 22) uniform sampler2D inputTex;

layout(binding = 25) uniform samplerCube coverageDistTex;
layout(binding = 26) uniform samplerCube shadowsTex;
layout(binding = 27) uniform samplerCube skyfogTex;
layout(binding = 23) uniform sampler2D waterTileTex;

uniform int UseAO;
uniform float T100;
uniform float T001;

uniform int CombineStep;
#define STEP_PREVIOUS_SUN 0
#define STEP_WATER_CLOUDS 1



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
float roughnessToMipmap(float roughness, samplerCube txt){
    //roughness = roughness * roughness;
    float levels = max(0, float(textureQueryLevels(txt)));
    float mx = log2(roughness*1024+1)/log2(1024);
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
    for(int i=0;i<17;i++){
        vec3 rdp = normalize(dir + randpoint3() * blurrange);
        float there = textureLod(cloudsCloudsTex, rdp, mlvel).g;
        float w =  clamp(1.0 / (abs(there - dst)*0.001 + 0.001), 0.0, 1.0);
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
    //ret.w = textureLod(skyfogTex, dir, mlvel).r;
   // ret.w = blurskyfog(dir, roughness);
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
float hitdist = 0;
#define waterdepth 1.0 * WaterWavesScale
#define WATER_SAMPLES_LOW 2
#define WATER_SAMPLES_HIGH 4
vec3 normalx(vec3 pos, float e){
    hitpos = pos;
    hitdist = distance(CameraPosition, pos);
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
    float rd = stepsize * rdo * 1.0;
    vec3 pos = upper;
    float h = 0.0;
    for(int i=0;i<stepsI + 1;i++){
        pos = mix(upper, lower, iter);
        h = heightwater(pos.xz, WATER_SAMPLES_LOW);
        if(h > 1.0 - (iter )) {
            iter -= stepsize;
            stepsize = stepsize / 4.0;
            for(int g=0;g<4;i++){
                pos = mix(upper, lower, iter);
                h = heightwater(pos.xz, WATER_SAMPLES_LOW);
                if(h > 1.0 - (iter)) {
                    return normalx(pos, 0.1);
                }
                iter += stepsize;
            }
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



float getthatfuckingfresnel(float reflectivity, vec3 normal, vec3 cameraSpace, float roughness){
    vec3 dir = normalize(reflect(normalize(cameraSpace), normal));
    float base =  1.0 - abs(dot(normalize(normal), dir));
    float fresnel = (reflectivity + (1-reflectivity)*(pow(base, mix(5.0, 0.8, roughness))));
    float angle = 1.0 - base;
    return fresnel * (1.0 - roughness);
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
    return pow(dt*dt*dt*dt, 112.0 * gloss*gloss + 11.0) * (200.0 * gloss + 10.0) * smoothstep(-0.02, -0.01, sundir.y);
   // return smoothstep(0.997, 1.0, dt);
}
vec3 shadingWater(PostProceessingData data, vec3 lightDir, vec3 colorA, vec3 colorB){
    float fresnel = getthatfuckingfresnel(0.04, data.normal, normalize(data.cameraPos), 0.0);
    return sun(data.normal, SunDirection, 1.0 - data.roughness , data.roughness) * 1.0 * colorA * fresnel + colorB * (1.0 - fresnel);
}

vec3 shadingNonMetalic(PostProceessingData data, vec3 lightDir, vec3 color){
    float fresnel = getthatfuckingfresnel(0.04, data.normal, normalize(data.cameraPos), data.roughness);
    float x = 1.0 - max(0, dot(lightDir, data.normal));
    vec3 radiance = shade(CameraPosition, vec3(fresnel), data.normal, data.worldPos, data.worldPos - lightDir * 1.0, color * data.diffuseColor, max(0.004, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - data.roughness);    

    vec3 difradiance = shadeDiffuse(CameraPosition, data.diffuseColor * (1.0 - fresnel), data.normal, data.worldPos, data.worldPos - lightDir * 1.0, color, 0.0, false) * x;
    //   return vec3(0);
    return radiance + difradiance; 
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
    return c ;
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
    
vec3 getDiffuseAtmosphereColor(){
    return textureLod(atmScattTex, vec3(0, 1, 0), textureQueryLevels(atmScattTex) - 1).rgb;
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
    vec3 diffuse = getDiffuseAtmosphereColor() * 0.4;
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

vec3 shadeFragment(PostProceessingData data){
    vec3 suncolor = getSunColor(0.0);
    vec3 sun = shadeColor(data, -SunDirection, suncolor);
    vec3 diffuse = shadeColor(data, -data.normal, sampleAtmosphere(data.normal, data.roughness*0.5, 1.0)) * 0.2;
    return sun * CSMQueryVisibility(data.worldPos) + diffuse;
}

vec3 getNormalLighting(vec2 uv, PostProceessingData data){
    if(length(data.normal) < 0.01){
        data.roughness = 0.0;
        return sampleAtmosphere(reconstructCameraSpaceDistance(uv, 1.0), 0.0, 1.0);
    } else {
        return shadeFragment(data);
    }
}

vec3 integrateStepsAndSun(){
    currentData.cameraDistance = length(currentData.normal) < 0.01 ? 999999.0 : currentData.cameraDistance;
    return getNormalLighting(UV, currentData);
}

vec3 integrateCloudsWater(){
    return textureLod(waterColorTex, UV, 0.0).rgb;
}


vec4 shade(){    
    vec3 color = vec3(0);
    if(CombineStep == STEP_PREVIOUS_SUN){
        color = integrateStepsAndSun();
    } else {
        color = integrateCloudsWater();
    }
    return vec4( clamp(color, 0.0, 10.0), currentData.cameraDistance * 0.001);
}