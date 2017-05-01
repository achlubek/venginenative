#ifndef RESOLVEATMOSPHERE_H
#define RESOLVEATMOSPHERE_H
#include CSM.glsl
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


#include ShadeFramework.glsl

//layout(binding = 14) uniform sampler2D fresnelTex;
float fresneleffect(float base, float roughness, vec3 cameradir, vec3 normal){
    //return base + (1.0 - base) * textureLod(fresnelTex, vec2(clamp(  roughness, 0.01, 0.98), 1.0 - max(0.01, dot(-cameradir, normal))), 0.0).r;
    float dt = dot(-cameradir, normal);
    return base + (1.0 - base) * textureLod(fresnelTex, vec2(clamp(roughness, 0.01, 0.98), clamp(1.0 - dt, 0.01, 0.98)), 0.0).r * (1.0 - roughness);
}

float rdhash = 0.453451 + Time;
vec3 randpoint3(){
    float x = rand2s(UV * rdhash);
    rdhash += 2.1231255;
    float y = rand2s(UV * rdhash);
    rdhash += 1.6271255;
    float z = rand2s(UV * rdhash);
    rdhash += 1.1231255;
    return vec3(x, y, z) * 2.0 - 1.0;
}
float blurshadows(vec3 dir, float roughness){
    //if(CloudsDensityScale <= 0.010) return 0.0;
    float levels = max(0, float(textureQueryLevels(shadowsTex)));
    float mx = log2(roughness*1024+1)/log2(1024);
    float mlvel = mx * levels;
    float dst = textureLod(coverageDistTex, dir, mlvel).g;
    float aoc = 1.0;

    float centerval = textureLod(shadowsTex, dir, mlvel).r;
    float blurrange = 0.005;
    for(int i=0;i<7;i++){
        vec3 rdp = normalize(dir + randpoint3() * blurrange);
        float there = textureLod(coverageDistTex, rdp, mlvel).g;
        float w = clamp(1.0 / (abs(there - dst)*0.01 + 0.01), 0.0, 1.0);
        centerval += w * textureLod(shadowsTex, rdp, mlvel).r;
        aoc += w;
    }
    centerval /= aoc;
    return centerval;
}
float blurshadowsCV(vec3 dir, float roughness){
    //if(CloudsDensityScale <= 0.010) return 0.0;
    float levels = max(0, float(textureQueryLevels(shadowsTex)));
    float mx = log2(roughness*1024+1)/log2(1024);
    float mlvel = mx * levels;
    float dst = textureLod(coverageDistTex, dir, mlvel).g;
    float aoc = 1.0;

    float centerval = textureLod(coverageDistTex, dir, mlvel).r;
    float blurrange = 0.002;
    for(int i=0;i<7;i++){
        vec3 rdp = normalize(dir + randpoint3() * blurrange);
        float there = textureLod(coverageDistTex, rdp, mlvel).g;
        float w = clamp(1.0 / (abs(there - dst)*0.01 + 0.01), 0.0, 1.0);
        centerval += w * textureLod(coverageDistTex, rdp, mlvel).r;
        aoc += w;
    }
    centerval /= aoc;
    return centerval;
}

#define s2(a, b)                temp = a; a = min(a, b); b = max(temp, b);
#define mn3(a, b, c)            s2(a, b); s2(a, c);
#define mx3(a, b, c)            s2(b, c); s2(a, c);

#define mnmx3(a, b, c)          mx3(a, b, c); s2(a, b);                                   // 3 exchanges
#define mnmx4(a, b, c, d)       s2(a, b); s2(c, d); s2(a, c); s2(b, d);                   // 4 exchanges
#define mnmx5(a, b, c, d, e)    s2(a, b); s2(c, d); mn3(a, c, e); mx3(b, d, e);           // 6 exchanges
#define mnmx6(a, b, c, d, e, f) s2(a, d); s2(b, e); s2(c, f); mn3(a, b, c); mx3(d, e, f); // 7 exchanges
vec3 median(sampler2D tex, vec2 uv){
    vec2 Tinvsize = 1.0 / textureSize(tex, 0);
    vec3 v[9];

    // Add the pixels which make up our window to the pixel array.
    for(int dX = -1; dX <= 1; ++dX) {
        for(int dY = -1; dY <= 1; ++dY) {
            vec2 offset = vec2(float(dX), float(dY));
            v[(dX + 1) * 3 + (dY + 1)] = texture2D(tex, uv + offset * Tinvsize).rgb;
        }
    }

    vec3 temp;

    // Starting with a subset of size 6, remove the min and max each time
    mnmx6(v[0], v[1], v[2], v[3], v[4], v[5]);
    mnmx5(v[1], v[2], v[3], v[4], v[6]);
    mnmx4(v[2], v[3], v[4], v[7]);
    mnmx3(v[3], v[4], v[8]);
    return v[4];

}

vec4 blurshadowsAO(vec3 dir, float roughness){
    //if(CloudsDensityScale <= 0.010) return 0.0;
    float levels = max(0, float(textureQueryLevels(shadowsTex)));
    float mx = log2(roughness*1024+1)/log2(1024);
    float mlvel = mx * levels;
    return textureLod(shadowsTex, dir, mlvel).rgba;
    float dst = textureLod(coverageDistTex, dir, mlvel).g;
    float aoc = 1.0;

    vec4 centerval = textureLod(shadowsTex, dir, mlvel).rgba;
    float cluma = length(centerval);
    float blurrange = 0.18 * (1.0 - 1.0 / (dir.y * 1.0 + 1.0)) * abs(CloudsThresholdHigh - CloudsThresholdLow);
    for(int i=0;i<32;i++){
        vec3 rdp = normalize(dir + randpoint3() * blurrange);
        //float there = textureLod(coverageDistTex, rdp, mlvel).g;
        //float w = clamp(1.0 / (abs(there - dst)*0.01 + 0.01), 0.0, 1.0);
        vec4 th = textureLod(shadowsTex, rdp, mlvel).rgba;

        centerval += textureLod(shadowsTex, rdp, mlvel).rgba;
        aoc += 1.0;
    }
    centerval /= aoc;
    //return centerval;
    return centerval;
}
vec4 smartblur(vec3 dir, float roughness){
    float levels = max(0, float(textureQueryLevels(coverageDistTex)));
    float mx = log2(roughness*1024+1)/log2(1024);
    float mlvel = mx * levels;
    vec4 ret = vec4(0);
    ret.xy = textureLod(coverageDistTex, dir, mlvel).rg;
//  ret.x = blurshadowsCV(dir, roughness);
    ret.z = textureLod(shadowsTex, dir, mlvel).r;
    //ret.z = blurshadows(dir, roughness);
   // ret.w = textureLod(skyfogTex, dir, mlvel).r;
   // ret.w = blurskyfog(dir, roughness);
    return ret;
}

vec3 shadingMetalic(PostProcessingData data, vec3 lightDir, vec3 color){
    float fresnelR = fresneleffect(data.diffuseColor.r, data.roughness, normalize(data.cameraPos), data.normal);
    float fresnelG = fresneleffect(data.diffuseColor.g, data.roughness, normalize(data.cameraPos), data.normal);
    float fresnelB = fresneleffect(data.diffuseColor.b, data.roughness, normalize(data.cameraPos), data.normal);
    float fresnel  = fresneleffect(0.04               , 0.0, normalize(data.cameraPos), data.normal);
    vec3 newBase = vec3(fresnelR, fresnelG, fresnelB);
    //   return vec3(fresnel);
    float x = 1.0 - max(0, dot(lightDir, data.originalNormal));
    return newBase * color;//shade(CameraPosition, data.diffuseColor, data.normal, data.worldPos, data.worldPos - lightDir * 1.0, color,  max(0.004, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - data.roughness);
}
float sun(vec3 dir, vec3 sundir, float gloss, float ansiox){
  //  float dt2 = max(0, dot(normalize(vec3(dir.x, abs(sundir.y), dir.z)), sundir));
 //   float dty = 1.0 - max(0, abs(dir.y - sundir.y));
   // dt = mix(dt, dt2, ansio);
    //return dt;

    float ansio = pow(abs(dir.y), 2.0);
    //dir.y *= 0.1 + 0.9 * gloss * gloss;
    //sundir.y *= 0.1 + 0.9 * gloss * gloss;
   // dir.y *= 0.1 + 0.9 * ansio;
   // sundir.y *= 0.1 + 0.9 * ansio;
   // dir = normalize(dir);
   // sundir = normalize(sundir);
  // return gloss;
    float dt = clamp(dot(dir, sundir) + 0.0, 0.0, 1.0);
    return pow(dt, 521.0 * gloss * gloss * gloss * gloss * gloss + 111.0) * (101.0 * gloss + 1.0) * smoothstep(-0.02, -0.01, sundir.y);
   // return smoothstep(0.997, 1.0, dt);
}
vec3 getSunColorDirectly(float roughness){
    float dt = max(0.0, (dot(dayData.sunDir, VECTOR_UP)));
    float st = max(0.0, 1.0 - smart_inverse_dot(dt, 11.0));
    vec3 supersundir = max(vec3(0.0),   vec3(1.0) - st * 4.0 * pow(vec3(50.0/255.0, 111.0/255.0, 153.0/255.0), vec3(2.4)));
    return supersundir * 4.0 * smart_inverse_dot(dt, 11.0);
}

float sshadow = 1.0;
vec3 shadingWater(PostProcessingData data, vec3 n, vec3 lightDir, vec3 colorA, vec3 colorB, float shadow){
    float fresnel  = fresneleffect(0.04, 0.0, normalize(data.cameraPos), n);
    fresnel = mix(fresnel, 0.0, data.roughness);
    return colorB * ( fresnel) + shade_ray_data(data, dayData.sunDir, shadow *  getSunColorDirectly(0.0));
   // return  colorB * (  fresnel);
}


vec3 shadingNonMetalic(PostProcessingData data, vec3 lightDir, vec3 color){
    float fresnel  = fresneleffect(0.04               , data.roughness, normalize(data.cameraPos), data.normal);
    float x = 1.0 - max(0, dot(lightDir, data.normal));
    vec3 radiance = fresnel * shade(CameraPosition, vec3(data.diffuseColor), data.normal, data.worldPos, data.worldPos - lightDir * 1.0, color, max(0.004, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - data.roughness);

    vec3 difradiance = shadeDiffuse(CameraPosition, data.diffuseColor , data.normal, data.worldPos, data.worldPos - lightDir * 1.0, color, 0.0, false) * x;
    //   return vec3(0);
    return radiance + difradiance;
}

vec3 getDiffuseAtmosphereColor(){
    vec3 total = vec3(0);
    vec3 dif1  = -dayData.sunDir;
    dif1.y = abs(dif1.y);
    return textureLod(atmScattTex, vec3(0.0, 1.0, 0.0), textureQueryLevels(atmScattTex) - 2.0).rgb;
}

vec3 getSunColor(float roughness){
    float dt = pow(max(0.0, (dot(dayData.sunDir, VECTOR_UP))), 2.0);
    return dt * getSunColorDirectly(roughness) * 7.0;
}


vec3 getAtmosphereScattering(vec3 dir, float roughness){
    return textureLod(atmScattTex, dir, roughnessToMipmap(roughness, atmScattTex)).rgb;
}


vec3 shadeColor(PostProcessingData data, vec3 lightdir, vec3 c){
    return mix(shadingNonMetalic(data, lightdir, c), shadingMetalic(data, lightdir, c),1.0);// * (UseAO == 1 ? texture(aoxTex, UV).r : 1.0);
}

vec2 projectvdao(vec3 pos){
    vec4 tmp = (VPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}
vec4 projectvdao2(vec3 pos){
    vec4 tmp = (VPMatrix * vec4(pos, 1.0));
    return (tmp.xyzw);
}
vec3 getScatterColor(float dist){
	vec3 color = vec3(0.03, 0.05, 0.1) * dist * 0.00004;
	return max(pow(color, -color)*color, 0.0);
}
float godrays(vec3 dir, int steps){
    if(NoiseOctave1 <= 0.011) return 1.0;
    float bias = 58.0 / float(steps);
    float stepsize = 1.0 / float(steps);
    float rd = rand2sTime(UV) * stepsize;

    float iter = 0.0;
    float result = 1.0;
    vec3 sdir = dayData.sunDir;
    for(int i=0;i<steps;i++){
        result -= textureLod(coverageDistTex, normalize(mix(dir, sdir, iter + rd)), 0.0).r * stepsize;
        iter += stepsize;
    }
    return result;
}

vec2 xyzToPolar(vec3 xyz){
    float theta = atan(xyz.y, xyz.x);
    float phi = acos(xyz.z);
    return vec2(theta, phi) / vec2(2.0 *3.1415,  3.1415);
}


vec3 getStars(vec3 dir, float roughness){
    dir = normalize(dayData.viewFrame * dir);
    vec3 c = pow(textureLod(starsTex, vec2(1.0) - xyzToPolar(dir), 0.0).rrr * 3.0, vec3(5.0)) * (supernoise3dX(dir * 100.0 + Time * 5.0) * 0.8 + 0.2);
    return mix(c * 0.15, vec3(0.001, 0.002, 0.003) * 0.2 * (1.0  - reconstructCameraSpaceDistance(UV, 1.0).y * 0.8), 0.2);
}
mat3 calcLookAtMatrix(vec3 origin, vec3 target, float roll) {
  vec3 rr = vec3(sin(roll), cos(roll), 0.0);
  vec3 ww = normalize(target - origin);
  vec3 uu = normalize(cross(ww, rr));
  vec3 vv = normalize(cross(uu, ww));

  return mat3(uu, vv, ww);
}
float sun_moon_mult = 1.0;
vec3 textureMoon(vec3 dir){
  /*  vec3 mond = dayData.moonDir;
    vec3 mont = cross(vec3(0.0, 1.0, 0.0), mond);
    vec3 monb = cross(mont, mond);
    float dt = max(0.0, dot(dir, mond));
    vec3 dd = normalize(mond - dir);
    vec2 ud = vec2(dot(mont, dd), dot(monb, dd));
    float cr = 0.99885;
    dt = (dt - cr) / (1.0 - cr);
    float st = smoothstep(0.0, 0.01, dt);
    mond = normalize(dayData.moonPos - dayData.earthPos);
    Sphere moon = Sphere(mond * 362600.0, 17360.0);
    Ray r = Ray(CameraPosition, dir);
    float i = rsi2(r, moon);
    vec3 posOnMoon = CameraPosition + dir * i;
    vec3 n = normalize(posOnMoon - mond * 362600.0);
    float l = pow(max(0.0, dot(n, dayData.sunSpaceDir)), 1.2);
    return l * pow(textureLod(moonTex, clamp((sqrt(1.0 - dt) * ud) * 2.22 * 0.5 + 0.5, 0.0, 1.0), 0.0).rgb, vec3(2.4)) * 2.0 + getStars(dir, 0.0) * (1.0 - st);*/
    dir.y = abs(dir.y);
    Sphere moon = Sphere(dayData.moonPos, 35.37);
    Ray r = Ray(dayData.earthPos, dayData.viewFrame * dir);
    float i = rsi2(r, moon);
    vec3 posOnMoon = dayData.earthPos + dayData.viewFrame * dir * i;
    vec3 n = normalize(dayData.moonPos - posOnMoon);
    float l = 0.0005 + pow(max(0.0, dot(n, dayData.sunSpaceDir)), 1.2);
    n = calcLookAtMatrix(dayData.moonPos, dayData.earthPos, 0.0) * n;
    n *= rotationMatrix(vec3(0.0, 1.0, 0.0), 1.8415);
   // n *= rotationMatrix(vec3(0.0, 0.0, 1.0), -3.1415);
    vec3 color = vec3(0.0) + pow(textureLod(moonTex, xyzToPolar(n) , 2.0).rgb, vec3(2.4));

    vec3 atm = mix(vec3(0.3, 0.1, 0.0), vec3(1.0), 1.0 - pow(1.0 - max(0.0, dayData.moonDir.y * 1.1 - 0.1), 4.0));
    color *= atm;
    sun_moon_mult = step(0.0, i);

    float monsoonconverage =  smoothstep(0.9957, 0.9962, dot(dir, dayData.moonDir));
    return clamp(l * color * sun_moon_mult * monsoonconverage * 1.1 + getStars(dir, 0.0) * (1.0 - sun_moon_mult), 0.0, 2.0);
}



float thatsunglowIdontknownamefor(vec3 dir, float strength, float power){
    float d = max(0.0, dot(dir, dayData.sunDir));
    return 1.0 + pow(d, power) * strength;
}

vec3 sampleAtmosphere(vec3 dir, float roughness, float sun, int raysteps){
    float dimmer = max(0, 0.06 + 0.94 * dot(normalize(dayData.sunDir), vec3(0,1,0)));
    vec3 scattering = getAtmosphereScattering(dir, roughness);
    vec3 scattering2 = getAtmosphereScattering(dir, 0.2);
    vec3 moon = textureMoon(dir);
    float monsoonconverage = (1.0 - smoothstep(0.995, 1.0, dot(dayData.sunDir, dayData.moonDir))) * 0.99 + 0.01;
    float monsoonconverage2 = (1.0 - smoothstep(0.995, 0.996, dot(dir, dayData.moonDir)));
    vec4 cloudsData = smartblur(dir, roughness);
    float coverage = cloudsData.r;
    //scattering += monsoonconverage2 * (1.0 - smoothstep(0.0, 0.9, coverage)) * (lenssun(dir)) * getSunColorDirectly(0.0) * 11.0;
    sshadow = 1.0 - coverage;
    //float dist = cloudsData.g;
    vec4 vdao = blurshadowsAO(dir, roughness);
//    return vdao.gba;
    float shadow = vdao.r;
    //return ;
    float rays = godrays(dir, raysteps);

    // hmm
    vec3 SunC = getSunColorDirectly(roughness);// * max(0.0, dot(VECTOR_UP, dayData.sunDir));
    vec3 AtmDiffuse = getDiffuseAtmosphereColor();
    float Shadow = shadow;

    //return coverage * vec3(SunC * Shadow);
    vec3 GroundC = vec3(0.5, 0.5, 0.5) * max(0.0, dot(dayData.sunDir, VECTOR_UP));
    float Coverage = 1.0 - smoothstep(0.4, 0.55, CloudsThresholdLow);//texture(coverageDistTex, VECTOR_UP, textureQueryLevels(coverageDistTex)).r;
    //vec2 aabbdd = blurshadowsAO(dir, roughness);
    float AOGround = 1.0;//aabbdd.r;
    float AOSky = 0.15;//aabbdd.g;

    float SunDT = max(0.0, dot(dayData.sunDir, VECTOR_UP));
    float DirDT = max(0.0, dot(dir, VECTOR_UP));
    float invDirDT = 1.0 / DirDT;

    #define xA 0.5
    #define xB 0.5
    float mult = mix(sqrt(0.001 + dot(dayData.sunDir, dir) * 0.5 + 0.5), 1.0, SunDT) + 0.02;
    vec3 raycolor = (getSunColorDirectly(0.0) * 4.9 * rays + (AtmDiffuse * 0.2 ) * 0.8) + AtmDiffuse ;

    //raycolor *= xA + xB * (pow(1.0 - DirDT, 8.0));
    float raysCoverage = clamp((1.0 / (DirDT * 61.0 / NoiseOctave1 + 1.0)), 0.0, 1.0);
    //return vdao;
    //return vdao.gba;
    //return lightnings;
    vec3 CC = vdao.gba;

    scattering *= xA + xB * (1.0 - pow(1.0 - DirDT, 14.0));
//    CC = mix(scattering * 0.7, CC, xA + xB * (1.0 - pow(1.0 - DirDT, 14.0)));
    CC *= xA + xB * (1.0 - pow(1.0 - DirDT, 14.0));


    return mix(mix(scattering * monsoonconverage + moon, monsoonconverage * CC, coverage), raycolor, raysCoverage);

    /*
    direct += (1.0 - smoothstep(0.0, 0.3, shadow)) * 0.1 * thatsunglowIdontknownamefor(dir, 6.0, 8.0);

    vec3 cloud = mix(diffuse * (getCloudsAO(dir) ) + raycolor * rays, direct , shadow);
    return mix(scattering  * monsoonconverage + moon + raycolor * rays, monsoonconverage * cloud, coverage);*/
}

vec2 projectvdaox(vec3 pos){
    vec4 tmp = (VPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}

layout(binding = 14) uniform sampler2D someLastPass;
vec2 project_abovex(vec3 pos){
    vec2 a = ((pos - CameraPosition).xz / AboveSpan) * 0.5 + 0.5;
    a.y = 1.0 - a.y;
    a.x = 1.0 - a.x;
    return a;
}
vec3 ssao(vec3 p1){
    vec3 v = vec3(0.0);
    float v2 = 0.0;
    float iter = 0.0;
    vec2 uv = UV ;
    float rd = rand2s(uv * Time) * 12.1232343456 ;
    float targetmeters = 0.7;
    int steps = 2;
    float twopi = 3.1415 * 2.0;
    float stepsize = 1.0 / 2.0;
    vec3 refl = normalize(reflect(reconstructCameraSpaceDistance(UV, 1.0), currentData.normal));
    for(int i=0;i<steps;i++){
        vec3 vx = vec3(0.0);
        vx.x = rand2s(uv + rd);
        uv.x += 1.46456;
        vx.y = rand2s(uv + rd);
        uv.x += 1.234234;
        vx.z = rand2s(uv + rd);
        uv.x += 1.187934;
        vx = vx * 2.0 - 1.0;
        rd += 1.234;
        vx = normalize(vx);
        vx *= sign(dot(vx, currentData.normal));
        vx = normalize(mix(vx, refl, 1.0 - currentData.roughness));
        vec3 ppos = p1 + targetmeters * vx;
        vec3 ppos2 = p1 + targetmeters * 10.0 * vec3(vx.x, abs(vx.y), vx.z);
        vec2 u = projectvdao(ppos);

        iter = 0.0 + rand2sTime(u) * 0.25;
        vec3 vis = vec3(0.0);
        for(int g=0;g<4;g++){
            vec3 posh = mix(p1, ppos, iter);
            vec3 posh2 = mix(p1, ppos2, iter );
            vec2 coords = clamp(projectvdao(posh), 0.0, 1.0);
            float dst = textureLod(mrt_Distance_Bump_Tex, coords, 2.0).r;
            vec3 newpos = CameraPosition + normalize(posh - CameraPosition) * dst;
            vis += 1.0;// - (1.0 - smoothstep(0.0, targetmeters, distance(newpos, p1))) * max(0.0, dot(currentData.normal, normalize(newpos - p1)));

            vec2 aboveuv = project_abovex(posh2);
            float v = textureLod(aboveViewDataTex, aboveuv, 0.0).r;
            float cancel =  ( 1.0 - smoothstep(0.0, 10.0, abs(v - p1.y)));
            vis += (1.0 - clamp(max(0.0, v - p1.y) , 0.0, 1.0))  * max(0.0, vx.y);
            iter += 0.25;
        }
        vis *= 0.25  * 0.5;
        v += vis;

    }
    return pow(v * stepsize, vec3(1.0 + 3.0 * (1.0 - currentData.roughness)));
}

vec2 traceReflectionX(vec3 pos, vec3 dir){
    vec2 res = vec2(-1.0);
    float score = 0.9992;

    vec2 uv = projectvdaox(pos);
    vec2 uvd = normalize(projectvdaox(pos + dir * 0.1) - uv);

    vec2 e = 2.0 / Resolution;
    for(int i=0;i<444;i++){
        float dd = textureLod(mrt_Distance_Bump_Tex, uv, 0).r;
        if(dd > 0.0){
            vec3 p = CameraPosition + reconstructCameraSpaceDistance(uv, dd);
            float dt = dot(dir, normalize(p - pos));
            if(score < dt) { score = dt; res = uv; }
        }
        uv += uvd * e;
        if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) break;
    }
    return res;
}

float traceReflection(vec3 pos, vec3 dir){
    vec2 res = vec2(-1.0);
    float score = 0.9992;

    vec2 uv = projectvdaox(pos);
    vec3 vdir = reconstructCameraSpaceDistance(uv, 1.0);
    float horizon = projectvdaox(CameraPosition + vec3(vdir.x, 0.0, vdir.z)).y * 2.0 - 1.0;

    return textureLod(mrt_Distance_Bump_Tex, vec2(uv.x, horizon - uv.y), 0).r;
}
float AboveAO = 1.0;
vec3 vdao(){
    vec3 c = vec3(0.0);
    int steps = 5;
    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
    //float fresnel = 0.04 + 0.96 * textureLod(fresnelTex, vec2(clamp(currentData.roughness, 0.01, 0.98), 1.0 - max(0.0, dot(-dir, currentData.normal))), 0.0).r;
    vec3 refdir = reflect(dir, currentData.normal) * (0.01 + rand2sTime(UV));
    vec3 atmdiff = textureLod(resolvedAtmosphereTex, currentData.normal, 9.0).rgb;
    for(int i=0;i<steps;i++){
        vec3 p = normalize(randpoint3());
        p *= sign(dot(p, currentData.normal));
    //    p = normalize(mix(p, refdir,  max(0.0, dot(p, refdir))));
        p = mix(refdir, p, currentData.roughness * currentData.roughness);
        //vec3 x = -p;
        //float v = mix(0.1 + 0.8 * smoothstep(-0.5, 0.0, p.y), max(0.0, dot(VECTOR_UP, currentData.normal)), currentData.roughness);


        c += 0.2 * shade_ray_env_data(currentData, p,  textureLod(resolvedAtmosphereTex, p, roughnessToMipmap(currentData.roughness * currentData.roughness * 0.3, resolvedAtmosphereTex)).rgb, atmdiff);
    }

    //return c;
    //return vec3(1) * ssao(currentData.worldPos);
    return shade_ray_data(currentData, dayData.sunDir, CSMQueryVisibility(currentData.worldPos)  * getSunColorDirectly(0.0)) + c * (0.06 + 0.94 * AboveAO) * ssao(currentData.worldPos);
}

vec3 shadeFragment(PostProcessingData data){
    vec3 suncolor = getSunColor(0.0);
    vec3 sun = shadeColor(data, -dayData.sunDir, suncolor);
    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
    vec3 mx = normalize(mix(reflect(dir, data.normal), data.normal, data.roughness));
    vec3 diffuse = vdao();
    return diffuse;
}



vec3 getNormalLighting(vec2 uv, PostProcessingData data){
    if(length(data.normal) < 0.01){
        data.roughness = 0.0;
        vec3 dir = reconstructCameraSpaceDistance(uv, 1.0);
        return textureLod(resolvedAtmosphereTex, dir, 0.0).rgb ;
    } else {
        return shadeFragment(data);
    }
}
#endif
