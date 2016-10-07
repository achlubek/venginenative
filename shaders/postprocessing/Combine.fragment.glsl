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

uniform int CombineStep;
#define STEP_PREVIOUS_SUN 0
#define STEP_WATER_CLOUDS 1

#include PostProcessEffectBase.glsl
#include Atmosphere.glsl
#include noise2D.glsl
#include CSM.glsl

float roughnessToMipmap(float roughness, samplerCube txt){
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
    
vec3 getDiffuseAtmosphereColor(){
    return textureLod(atmScattTex, SunDirection, textureQueryLevels(atmScattTex) - 1 ).rgb;
}

vec3 getSunColor(float roughness){
    vec3 atm = textureLod(atmScattTex, SunDirection, roughnessToMipmap(roughness, atmScattTex)).rgb * 1.0;
    return mix((atm * 0.2)  , vec3(10.0), max(0.00, SunDirection.y)) * max(0.0, SunDirection.y);
}

vec3 getAtmosphereScattering(vec3 dir, float roughness){
    return textureLod(atmScattTex, dir, roughnessToMipmap(roughness, atmScattTex)).rgb;
}


vec3 shadeColor(PostProceessingData data, vec3 lightdir, vec3 c){
    return mix(shadingNonMetalic(data, lightdir, c), shadingMetalic(data, lightdir, c), data.metalness);// * (UseAO == 1 ? texture(aoxTex, UV).r : 1.0);
}

/*


        float diminisher = max(0, dot(normalize(SunDirection), vec3(0,1,0)));
        vec3 shadowcolor = mix(skydaylightcolor, skydaylightcolor * 0.05, 1.0 - diminisher);
        vec3 litcolor = mix(vec3(10.0), atmcolor * 0.2, 1.0 - diminisher);
        vec3 colorcloud = mix(shadowcolor, litcolor, pow(cdata.g, 2.0)) ;*/

vec3 sampleAtmosphere(vec3 dir, float roughness, float sun){
    float dimmer = max(0, 0.2 + 0.8 * dot(normalize(SunDirection), vec3(0,1,0)));
    vec3 scattering = getAtmosphereScattering(dir, roughness);
    vec3 diffuse = getDiffuseAtmosphereColor();
    vec3 direct = getSunColor(roughness);
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