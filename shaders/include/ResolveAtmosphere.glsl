#ifndef RESOLVEATMOSPHERE_H
#define RESOLVEATMOSPHERE_H
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
    float blurrange = 0.008 * dir.y;
    for(int i=0;i<12;i++){
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
    float levels = max(0, float(textureQueryLevels(cloudsCloudsTex)));
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
float sshadow = 1.0;
vec3 shadingWater(PostProcessingData data, vec3 n, vec3 lightDir, vec3 colorA, vec3 colorB){
    float fresnel  = fresneleffect(0.02, 0.0, normalize(data.cameraPos), n);
    fresnel = mix(fresnel, 0.05, data.roughness);
    return colorB * ( fresnel);
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
    return textureLod(atmScattTex, dif1, textureQueryLevels(atmScattTex) - 1.0).rgb;
}

vec3 getSunColorDirectly(float roughness){
    vec3 sunBase = vec3(15.0);
    float dt = max(0.0, (dot(dayData.sunDir, VECTOR_UP)));
    float dt2 = 0.9 + 0.1 * (1.0 - max(0.0, (dot(dayData.sunDir, VECTOR_UP))));
    vec3 supersundir = 25.0 * textureLod(atmScattTex, vec3(dayData.sunDir.x, max(0.0, dayData.sunDir.y * 0.1), dayData.sunDir.z), 1.0).rgb ;

    return mix(supersundir, sunBase, 1.0 - pow(1.0 - dt, 4.0));
    //return  max(vec3(0.3, 0.3, 0.0), (  sunBase - vec3(5.5, 18.0, 20.4) *  pow(1.0 - dt, 8.0)));
}

vec3 getSunColor(float roughness){
    float dt = pow(max(0.0, (dot(dayData.sunDir, VECTOR_UP))), 2.0);
    return dt * getSunColorDirectly(roughness);
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

float godrays(vec3 dir, int steps){
    if(NoiseOctave1 <= 0.011) return 1.0;
    float bias = 58.0 / float(steps);
    float stepsize = 1.0 / float(steps);
    float rd = rand2sTime(UV) * stepsize;

    float iter = 0.0;
    float result = 1.0;
    vec3 sdir = dayData.sunDir;
    float x = sdir.y * 0.5 + 0.5;
    x *= 0.94;
    x = x * 2.0 - 1.0;
    sdir.y = mix(x, sdir.y, sdir.y);
    for(int i=0;i<steps;i++){
        result -= textureLod(coverageDistTex, normalize(mix(dir, sdir, iter + rd)), 0.0).r * stepsize;
        iter += stepsize;
    }
    return result;
}

vec2 xyzToPolar(vec3 xyz){
    float radius = sqrt(xyz.x * xyz.x + xyz.y * xyz.y + xyz.z * xyz.z);
    float theta = atan(xyz.y, xyz.x);
    float phi = acos(xyz.z / radius);
    return vec2(theta, phi) / vec2(2.0 *3.1415,  3.1415);
}


vec3 getStars(vec3 dir, float roughness){
    dir = dayData.viewFrame * dir;
    vec3 c = pow(texture(starsTex, xyzToPolar(dir)).rgb, vec3(2.2));
    return mix(c * 0.1, vec3(0.001, 0.002, 0.003) * 0.2 * (1.0  - reconstructCameraSpaceDistance(UV, 1.0).y * 0.8), 0.0);
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
    Sphere moon = Sphere(dayData.moonPos, 17.37);
    Ray r = Ray(dayData.earthPos, dayData.viewFrame * dir);
    float i = rsi2(r, moon);
    vec3 posOnMoon = dayData.earthPos + dayData.viewFrame * dir * i;
    vec3 n = normalize(dayData.moonPos - posOnMoon);
    float l = pow(max(0.0, dot(n, dayData.sunSpaceDir)), 1.2);
    n = calcLookAtMatrix(dayData.moonPos, dayData.earthPos, 0.0) * n;
    n *= rotationMatrix(vec3(0.0, 1.0, 0.0), 1.8415);
   // n *= rotationMatrix(vec3(0.0, 0.0, 1.0), -3.1415);
    vec3 color = vec3(0.0) + pow(textureLod(moonTex, xyzToPolar(n) , 2.0).rgb, vec3(2.4));

    vec3 atm = mix(vec3(0.3, 0.1, 0.0), vec3(1.0), 1.0 - pow(1.0 - max(0.0, dayData.moonDir.y * 1.1 - 0.1), 4.0));
    color *= atm;
    sun_moon_mult = step(0.0, i);
    float monsoonconverage =  smoothstep(0.9990, 0.9993, dot(dir, dayData.moonDir));
    return clamp(l * color * sun_moon_mult * monsoonconverage * 1.1 + getStars(dir, 0.0) * (1.0 - sun_moon_mult), 0.0, 2.0);
}


float lenssun(vec3 dir){
    //return smoothstep(0.997, 0.999, dot(dir, dayData.sunDir));
    vec3 sdir = dayData.sunDir;
    float x = sdir.y * 0.5 + 0.5;
    x *= 0.94;
    x = x * 2.0 - 1.0;
    sdir.y = mix(x, sdir.y, sdir.y);
    vec2 ss1 = projectvdao(CameraPosition + dir);
    vec2 ss2 = projectvdao(CameraPosition + sdir);
    ss1.x *= Resolution.x / Resolution.y;
    ss2.x *= Resolution.x / Resolution.y;

    vec3 differente = normalize(dir - sdir) * 4.0;
    //return smoothstep(0.997, 0.998, dot(dir, dayData.sunDir));// fuck it pow(1.0 / (distance(dir, dayData.sunDir) * 22.0 - 0.05), 5.0);
    float primary = pow(1.0 / abs((distance(dir, sdir) * 22.0 - 0.05)), 3.0 + supernoise3dX(differente * 1.3 + Time * 0.2) * 1.0);

    return primary;
}

float thatsunglowIdontknownamefor(vec3 dir, float strength, float power){
    float d = max(0.0, dot(dir, dayData.sunDir));
    return 1.0 + pow(d, power) * strength;
}

vec3 sampleAtmosphere(vec3 dir, float roughness, float sun, int raysteps){
    float dimmer = max(0, 0.06 + 0.94 * dot(normalize(dayData.sunDir), vec3(0,1,0)));
    vec3 scattering = getAtmosphereScattering(dir, roughness);
    vec3 moon = textureMoon(dir);
    float monsoonconverage = (1.0 - smoothstep(0.995, 1.0, dot(dayData.sunDir, dayData.moonDir))) * 0.99 + 0.01;
    float monsoonconverage2 = (1.0 - smoothstep(0.995, 0.996, dot(dir, dayData.moonDir)));
    vec4 cloudsData = smartblur(dir, roughness);
    float coverage = cloudsData.r;
    scattering += monsoonconverage2 * (1.0 - smoothstep(0.0, 0.9, coverage)) * (lenssun(dir)) * getSunColorDirectly(0.0) * 11.0;
    sshadow = 1.0 - coverage;
    //float dist = cloudsData.g;
    vec4 vdao = blurshadowsAO(dir, roughness);
    //return vdao.gba;
    float shadow = vdao.r;
    //return ;
    float rays = godrays(dir, raysteps);

    // hmm
    vec3 SunC = getSunColor(roughness);// * max(0.0, dot(VECTOR_UP, dayData.sunDir));
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
    vec3 raycolor = mult * getSunColor(0.0) * NoiseOctave1 * 0.9;
    //raycolor *= xA + xB * (pow(1.0 - DirDT, 8.0));
    float raysCoverage = min(1.0, (0.05 + 0.95 * pow((1.0 - (asin(DirDT) / (3.1415 * 0.5)) ), 13.0) * NoiseOctave1 * 0.1));
    //return vdao;
    //return vdao.gba;
    vec3 CC = vdao.gba * mix(Shadow, 1.0, SunDT * 0.8 + 0.2) + (SunC * Shadow * mix(0.0, Shadow, SunDT * 0.8 + 0.2)) + (GroundC);

    scattering *= xA + xB * (1.0 - pow(1.0 - DirDT, 14.0));
//    CC = mix(scattering * 0.7, CC, xA + xB * (1.0 - pow(1.0 - DirDT, 14.0)));
    CC *= xA + xB * (1.0 - pow(1.0 - DirDT, 14.0));


    return mix(mix(scattering * monsoonconverage + moon, monsoonconverage * CC, coverage), raycolor * rays, raysCoverage);

    /*
    direct += (1.0 - smoothstep(0.0, 0.3, shadow)) * 0.1 * thatsunglowIdontknownamefor(dir, 6.0, 8.0);

    vec3 cloud = mix(diffuse * (getCloudsAO(dir) ) + raycolor * rays, direct , shadow);
    return mix(scattering  * monsoonconverage + moon + raycolor * rays, monsoonconverage * cloud, coverage);*/
}

vec2 projectvdaox(vec3 pos){
    vec4 tmp = (VPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}

float visibility(vec3 p1, vec3 p2){
    float v = 1.0;
    int steps = 16;
    float stepsize = 1.0 / 16.0;
    float iter = 0.0;
    float rd = stepsize * rand2s(UV * Time);
    vec2 u1 = UV;//projectvdaox(p1);
    vec2 u2 = projectvdaox(p2);
    float w = 1.0;
    for(int i=0;i<steps;i++){
        vec2 u = mix(u1, u2, iter + rd);
        vec3 p = mix(p1, p2, iter + rd);
        float dst = textureLod(mrt_Distance_Bump_Tex, u, 0.0).r;
        //dst = mix(99999.0, dst, step(0.1, dst));
        if(dst > 0.0)
        v -= stepsize * w * clamp(distance(p, CameraPosition) - dst, 0.0, 1.0);
        w -= stepsize;
        iter += stepsize;
    }
    return pow(v, 8.0);
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

vec3 vdao(){
    vec3 c = vec3(0.0);
    int steps = 11;
    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
    //float fresnel = 0.04 + 0.96 * textureLod(fresnelTex, vec2(clamp(currentData.roughness, 0.01, 0.98), 1.0 - max(0.0, dot(-dir, currentData.normal))), 0.0).r;
    //for(int i=0;i<steps;i++){
        vec3 p = normalize(randpoint3());
        p = currentData.normal;
        p = normalize(mix(reflect(dir, currentData.normal), p, currentData.roughness));
        vec3 x = -p;
        float v = mix(0.1 + 0.8 * smoothstep(-0.5, 0.0, p.y), max(0.0, dot(VECTOR_UP, currentData.normal)), currentData.roughness);// visibility(currentData.worldPos, currentData.worldPos + p * 1.0);

        p.y = abs(p.y);
        c += v * shade_ray_data(currentData, -x,  textureLod(resolvedAtmosphereTex, p, roughnessToMipmap(currentData.roughness * 1.0, resolvedAtmosphereTex)).rgb);
    //}
    return c;// / float(steps);
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
        return texture(resolvedAtmosphereTex, dir).rgb ;
    } else {
        return shadeFragment(data);
    }
}
#endif
