
uniform float CloudsFloor;
uniform float CloudsCeil;
uniform float CloudsThresholdLow;
uniform float CloudsThresholdHigh;
uniform float CloudsWindSpeed;
uniform vec3 CloudsOffset;
uniform float NoiseOctave1;
uniform float NoiseOctave2;
uniform float NoiseOctave3;
uniform float NoiseOctave4;
uniform float NoiseOctave5;
uniform float NoiseOctave6;
uniform float NoiseOctave7;
uniform float NoiseOctave8;
uniform float CloudsIntegrate;
uniform float AtmosphereScale;
uniform float CloudsDensityScale;
uniform float CloudsDensityThresholdLow;
uniform float CloudsDensityThresholdHigh;
uniform float Time;
uniform float WaterWavesScale;
uniform float Rand1;
uniform float Rand2;
uniform float MieScattCoeff;

#export float WindBigScale
#export float WindBigPower
#export float FBM1
#export float FBM2
#export float FBMSCALE


layout(binding = 18) uniform samplerCube cloudsCloudsTex;
layout(binding = 19) uniform samplerCube atmScattTex;
layout(binding = 29) uniform samplerCube mainPassTex;
//layout(binding = 22) uniform sampler2D atmScattTex;
layout(binding = 20) uniform samplerCube cloudsRefShadowTex;

#include Shade.glsl
#include noise3D.glsl
#include Constants.glsl
#include PlanetDefinition.glsl
#include ProceduralValueNoise.glsl


float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{ return dot(point - origin, normal) / dot(direction, normal); }

vec3 getAtmosphereForDirection(vec3 dir, float roughness){
    float levels = max(0, float(textureQueryLevels(atmScattTex)));
    float mx = log2(roughness*1024.0+1.0)/log2(1024.0);
    float mlvel = mx * levels;
    return textureLod(atmScattTex, dir, mlvel).rgb;
}

#define ssnoise(a) (snoise(a) * 0.5 + 0.5)

/*
#define xdnoise(a) ssnoise(a + ssnoise(a * 0.5) * 0.5)

float hcl = 0.0;
float cloudsDensity3DXXXX(vec3 pos){
    hcl = 1.0 - smoothstep(CloudsFloor, CloudsCeil, length(vec3(0, planetradius, 0) + pos) - planetradius);
    //pos += (getWind(pos * 0.0005  * WindBigScale) * WindBigPower * 1.3 + getWind(pos * 0.00155  * WindSmallScale) * WindSmallPower * 0.1) * 2000.0 + CloudsOffset * 100.0;
    pos += (getWind(pos * 0.0005  * WindBigScale) * WindBigPower * 1.3 + getWind(pos * 0.00155  * WindSmallScale) * WindSmallPower * 0.1) * 2000.0 + CloudsOffset * 100.0;
    float partitions = ssnoise(pos * 0.00001 * vec3(FBMINITSCALE, FBMINITSCALE, FBMINITSCALE));
    float partitions2 = ssnoise(pos * 0.00004 * vec3(FBMINITSCALE2, FBMINITSCALE2, FBMINITSCALE2));
    float aza = smoothstep(0.0, 0.1, hcl) * (1.0 - smoothstep(0.8, 1.0, hcl));
    partitions = (partitions * partitions2) ;
    float fao1 = FBMO1 * 0.1;
    float fao2 = FBMO2 * 0.1;
    //float localaberations = mix(1.0, xdnoise(pos * 0.0001 * FBMS1) * fao1 + 1.0 * (1.0 - fao1), hcl);
    float localaberations = ssnoise(pos * 0.0001 * FBMS1) * fao1 + 1.0 * (1.0 - fao1);
    //float localaberations2 = mx(1.0, xdnoise(pos * 0.0001 * FBMS1) * fao1 + 1.0 * (1.0 - fao1), hcl);
    float localaberations2 = ssnoise(pos * 0.001 * FBMS2).x * fao2 + 1.0 * (1.0 - fao2);
    float density = partitions * localaberations * localaberations2;
    return smoothstep(
        CloudsThresholdLow,
        CloudsThresholdHigh,
        density * aza);
} */
#define fbm fbm_alu
float fbm(vec3 p){
    p *= 0.001 * FBMSCALE;
    float a = 0.0;
    float w = 1.0;
    float wc = 0.0;
    for(int i=0;i<4;i++){
        a += (0.5 + 0.5 *snoise(p)) * w;
        w *= FBM1 * 0.5;
        wc+=w;
        p = p * FBM2 * 4.0 ;
    }
    return a / wc;
}
#define xsupernoise3d(a) abs(0.5 - supernoise3d(a))*2.0
float fbmLOW(vec3 p){
   // p *= 0.1;
    p *= 0.011 * FBMSCALE;
    float a = 0.0;
    float w = 0.66;
    for(int i=0;i<7;i++){
        //p += noise(vec3(a));
        a += xsupernoise3d(p) * w;
        w *= 0.66;
        p = p * 2.0 ;
    }
    return a;// + noise(p * 100.0) * 11;
}

float fbmFronts(vec3 p){
   // p *= 0.1;
    p *= 0.011 * FBMSCALE;
    float a = 0.0;
    float w = 0.5;
    for(int i=0;i<4;i++){
        //p += noise(vec3(a));
        a += xsupernoise3d(p) * w;
        w *= 0.45;
        p = p * 3.0 ;
    }
    return smoothstep(0.1, 0.5, a);// + noise(p * 100.0) * 11;
}

float fbmHI(vec3 p){
   // p *= 0.1;
    p *= 0.011 * FBMSCALE;
    float a = 0.0;
    float w = 0.66;
    for(int i=0;i<9;i++){
        //p += noise(vec3(a));
        a += xsupernoise3d(p) * w;
        w *= 0.66;
        p = p * 2.0  ;
    }
    return a;// + noise(p * 100.0) * 11;
}
vec3 getWind(vec3 p){
    return (vec3(
        supernoise3d(p),
        supernoise3d(-p * 0.2),
        supernoise3d(p.zxy)
    ) * 2.0 - 1.0);// * (supernoise3d(p * 0.1) * 0.5 + 0.5);
}
float getHeightOverSea(vec3 p){
    vec3 atmpos = vec3(0.0, planetradius, 0.0) + p;
    return length(atmpos) - planetradius;
}

float getFronts(vec3 pos){
    return fbmFronts(pos * 0.0005);// * step(CloudsThresholdHigh, fbmLOW(ps * 0.005));
}

float cloudsDensity3D(vec3 pos){
    vec3 ps = pos +CloudsOffset * 1111;// + wtim;
    ps += (getWind(pos * 0.0005  * WindBigScale) * (WindBigPower / WindBigScale)) * 600.0;

    float density = fbmHI(ps * 0.005) * getFronts(pos);// * step(CloudsThresholdHigh, fbmLOW(ps * 0.005));
    float measurement = (CloudsCeil - CloudsFloor) * 0.5;
    float mediana = (CloudsCeil + CloudsFloor) * 0.5;
    float mlt = sqrt(sqrt( 1.0 - (abs( getHeightOverSea(pos) - mediana ) / measurement )));
    float init = smoothstep(CloudsThresholdLow, CloudsThresholdHigh, density * mlt);
    return  init;
}
float cloudsDensity3DLOW(vec3 pos){
    vec3 ps = pos +CloudsOffset * 1111;// + wtim;
    ps += (getWind(pos * 0.0005  * WindBigScale) * (WindBigPower / WindBigScale)) * 600.0;

    float density = fbmLOW(ps * 0.005) * getFronts(pos);// * step(CloudsThresholdHigh, fbmLOW(ps * 0.005));
    float measurement = (CloudsCeil - CloudsFloor) * 0.5;
    float mediana = (CloudsCeil + CloudsFloor) * 0.5;
    float mlt = sqrt(sqrt( 1.0 - (abs( getHeightOverSea(pos) - mediana ) / measurement )));
    float init = smoothstep(CloudsThresholdLow, CloudsThresholdHigh, density * mlt);
    return  init;
}

float rand2s(vec2 co){
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
float rand2sTime(vec2 co){
    co *= Time;
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
float hitfloorX = 0.0;
float getCloudsAO(vec3 dir, float dirmult){
    vec2 asd = textureLod(mainPassTex, dir, 0.0).rg;
    float center = asd.g + hitfloorX;
    vec3 c = vec3(0.0, 1.0, 0.0);
    vec3 point = c + dir * center;
    vec3 normal = normalize(mix(-normalize(vec3(0.0, planetradius, 0.0) + point), dayData.sunDir, dirmult));
    //vec3 normal = -normalize(vec3(0.0, planetradius, 0.0) + point);

    float sumao = 0.0;
    vec2 uv = UV + vec2(Rand1, Rand2);
    for(int i=0;i<8;i++){
        float x = rand2s(uv);
        //rd *= 2.7897;
        uv.x += 1.46456;
        float y = rand2s(uv);
        //rd *= 1.6271255;
        uv.x += 1.234234;
        float z = rand2s(uv);
        //rd *= 1.234211;
        uv.x += 1.567567;
        vec3 px = vec3(x, y, z) * 2.0 - 1.0;
        vec2 there = textureLod(mainPassTex, dir + px * 0.18 / (center * 0.0001) , 1.0).rg;
        vec3 p = c + (dir + px * 0.18 / (center * 0.0001) ) * (there.g + hitfloorX);
        float dst = distance(p, point);
        float occ = max(0.0, dot(normal, normalize(p - point)));
        sumao += 1.0 - clamp( occ / (dst * 0.0001), 0.0, 1.0);
    }

    return 1.0 - clamp(sumao / 8.0, 0.0, 1.0);
}

float visibility(vec3 dir1, vec3 dir2, float d1, float d2){
    float v = 1.0;
    float iter = 0.0;
    float stepsize = 1.0 / 10.0;
    float rd = stepsize * rand2s(UV * Time);
    iter += stepsize;
    for(int i=0;i<6;i++){
        vec3 md = mix(dir1, dir2, iter + rd);
        float dd = mix(d1, d2, iter + rd);
        float dx = textureLod(mainPassTex, md, 0.0).g + hitfloorX;
        if(dx < dd) return 0.0;
        iter += stepsize;
    }
    return v;
}


#include ResolveAtmosphere.glsl
float getAO(vec3 pos, float randomization, float weight);
float getAODIR(vec3 pos, vec3 dir, float weight);
vec3 getCloudsAL(vec3 dir){
    vec2 asd = textureLod(mainPassTex, dir, 0.0).rg;
    if(asd.r == 0) return vec3(0.0);
    float center = asd.g + hitfloorX;
    vec3 c = vec3(0.0, 1.0, 0.0);
    vec3 point = c + dir * center;

    vec3 sum = vec3(0.0);
    float r = Time;
    float vdt = max(0.0, dot(dayData.sunDir, VECTOR_UP));
    float vdt2 = 1.0 - vdt;
    vec2 uv = UV + vec2(Rand1, Rand2);
    float rd = rand2s(uv) * 12.1232343456;
    float mult = 1.0;//mix(sqrt(dot(dayData.sunDir, dir) * 0.5 + 0.5), 1.0, vdt) + 0.02;
    for(int i=0;i<14;i++){
        float x = rand2s(uv) * 2.0 - 1.0;
        //rd *= 2.7897;
        uv.x += 1.46456;
        float y = rand2s(uv) * 1.2 - 0.2;
        //rd *= 1.6271255;
        uv.x += 1.234234;
        float z = rand2s(uv) * 2.0 - 1.0;
        //rd *= 1.234211;
        uv.x += 1.567567;
        vec3 px = normalize(vec3(x, y, z));
        vec3 p = point + px * 7010.0;
        vec3 newdir = normalize(p - c);
        float v = visibility(dir, newdir, center, textureLod(mainPassTex, newdir, 0.0).g + hitfloorX);
        sum += (textureLod(atmScattTex, px, 1.0).rgb) * v * mult;// * pow(vdt2, 4.0);
    }

    float dao = (1.0 -  getCloudsAO(dir, 1.0 - vdt)) * pow(vdt, 2.0) * 3.0;
    float daox = max(0.0,  getCloudsAO(dir, 1.0  ) * 2.0 - 1.0)   * pow(vdt * 0.8 + 0.2, 2.0) ;
    float caa = getCloudsAO(dir, 0.0);
    float sao1 = mix(1.0, 1.0 / (max(1.0 , CloudsDensityScale) * max(1.0 , CloudsDensityScale)) * 0.5 + 0.5 * caa, min(1.0, CloudsDensityScale));
    float sao2 = mix(1.0, pow(caa, max(1.0 , CloudsDensityScale)), min(1.0, CloudsDensityScale));
    float mx = 1.0 - pow(1.0 - max(0.0, dot(dir, VECTOR_UP)), 3.0);
    float sao =  (mix(sao1, sao2, mx) * (1.0 - pow(1.0 - vdt, 8.0)) * pow(vdt * 0.97 + 0.03, 1.0) * 14.0) / ((CloudsCeil - CloudsFloor) * 0.0005 + 1.0);
    //sao *= 1.0 + pow(caa, 2.0) * 1.0 * pow(max(0.0, dot(dir, dayData.sunDir)) * max(0.0, dot(dir, VECTOR_UP)), 5.0);
    sao *= pow(max(0.0, dot(dayData.sunDir, VECTOR_UP)), 12.0);
    //float dshadow = getAODIR(point, px, 113.0);

    float coverage =  smoothstep(0.464, 0.6, CloudsThresholdLow);
//  return vec3(sao) ;
    return clamp(sum / 14.0 , 0.0, 111.0);// + pow(sao1 * 2.0 / CloudsDensityScale, 2.0) * getSunColor(0.0) * 1.2;// + sao * 3.0 + daox * 0.1 ;
    //return vec3(0.0) +  sao * 3.0;// + daox * 0.1 ;
}

Sphere sphere1;
Sphere sphere2;

float weightshadow = 1.1;
float internalmarchconservativeCoverageOnly(vec3 p1, vec3 p2, float weight){
    const int stepcount = 38;
    const float stepsize = 1.0 / float(stepcount);
    float iter = 0.0;
    float rd = rand2sTime(UV) * stepsize;
    float coverageinv = 1.0;
    float linear = distance(p1, mix(p1, p2, stepsize));
    float mult = weight * stepsize;
    for(int i=0;i<stepcount;i++){
        vec3 pos = mix(p1, p2, iter + rd);
        float clouds = cloudsDensity3DLOW(pos);
        coverageinv *= 1.0 - clouds * mult;
        iter += stepsize;
        if(coverageinv <= 0.0) break;
    }
    return clamp(coverageinv, 0.0, 1.0);// * max(0.0, dot(dayData.sunDir, normalize(p2 - p1)));//* (1.0 - smoothstep(14000.0, 19000.0, distance(p1, p2)));
}

vec3 CAMERA = vec3(0.0, 1.0, 0.0);

vec2 internalmarchconservative(vec3 p1, vec3 p2){
    int stepcount = 38;
    float stepsize = 1.0 / float(stepcount);
    float rd = fract(rand2sTime(UV)) * stepsize;
    float c = 0.0;
    float w = 0.0;
    float coverageinv = 1.0;
    vec3 pos = vec3(0);
    float clouds = 0.0;
    float godr = 0.0;
    float godw = 0.0;
    float depr = 0.0;
    float depw = 0.0;
    float iter = 0.0;
    vec3 lastpos = p1;
    //depr += distance(CAMERA, lastpos);
    depw += 1.0;
    float linear = distance(p1, mix(p1, p2, stepsize));
    for(int i=0;i<stepcount;i++){
        pos = mix(p1, p2, iter + rd);
        clouds = cloudsDensity3D(pos);

        coverageinv *= 1.0 - clouds;
        if(coverageinv <= 0.0) break;
        depr += step(0.99, coverageinv) * distance(lastpos, pos);
        lastpos = pos;
        iter += stepsize;
        //rd = fract(rd + iter * 124.345345);
    }
    if(coverageinv > 0.99) depr = distance(CAMERA, (p1 + p2) * 0.5) - distance(CAMERA, p1);
    float cv = 1.0 - clamp(coverageinv, 0.0, 1.0);
    return vec2(cv, depr);
}

float hash1x = UV.x + UV.y + Time;
vec3 randdir(){
    float x = rand2s(UV * hash1x);
    hash1x += 0.5451;
    float y = rand2s(UV * hash1x);
    hash1x += 0.62123;
    float z = rand2s(UV * hash1x);
    hash1x += 0.4652344;
    return (vec3(
        x, y, z
    ) * 2.0 - 1.0);
}

float getAO(vec3 pos, float randomization, float weight){
    //vec3 dir = normalize(dayData.sunDir);
    //vec3 dir = normalize(dayData.sunDir + randdir() * randomization);
    vec3 dir = normalize(dayData.sunDir + randdir() * randomization);
    Ray r = Ray(vec3(0,planetradius ,0) +pos, dir);
    float hitplanet = rsi2(r, planet);
    if(hitplanet > 0.0) return 0.0;
    float hitceil = rsi2(r, sphere2);
    float hitfloor = rsi2(r, sphere1);
    vec3 posceil = pos + dir * hitceil;
    vec3 posfloor = pos + dir * hitfloor;
    if(hitfloor > 0.0 && hitceil > 0.0 && hitfloor < hitceil) return internalmarchconservativeCoverageOnly(pos, posceil, weight);
    if(hitfloor > 0.0 && hitceil > 0.0 && hitfloor > hitceil) return internalmarchconservativeCoverageOnly(pos, posceil, weight);
    if(hitfloor > 0.0 && hitceil <= 0.0) return internalmarchconservativeCoverageOnly(pos, posfloor, weight);
    if(hitfloor <= 0.0 && hitceil > 0.0) return internalmarchconservativeCoverageOnly(pos, posceil, weight);
    return internalmarchconservativeCoverageOnly(pos, posceil, weight);
}
float getAODIR(vec3 pos, vec3 dir, float weight){
    //vec3 dir = normalize(dayData.sunDir);
    //vec3 dir = normalize(dayData.sunDir + randdir() * randomization);
    Ray r = Ray(vec3(0,planetradius ,0) +pos, dir);
    float hitplanet = rsi2(r, planet);
    if(hitplanet > 0.0) return 0.0;
    float hitceil = rsi2(r, sphere2);
    float hitfloor = rsi2(r, sphere1);
    vec3 posceil = pos + dir * hitceil;
    vec3 posfloor = pos + dir * hitfloor;
    if(hitfloor > 0.0 && hitceil > 0.0 && hitfloor < hitceil) return internalmarchconservativeCoverageOnly(pos, posceil, weight);
    if(hitfloor > 0.0 && hitceil > 0.0 && hitfloor > hitceil) return internalmarchconservativeCoverageOnly(pos, posceil, weight);
    if(hitfloor > 0.0 && hitceil <= 0.0) return internalmarchconservativeCoverageOnly(pos, posfloor, weight);
    if(hitfloor <= 0.0 && hitceil > 0.0) return internalmarchconservativeCoverageOnly(pos, posceil, weight);
    return internalmarchconservativeCoverageOnly(pos, posceil, weight);
}

float shadows(){
    vec3 viewdir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    vec3 atmorg = vec3(0,planetradius,0) + CAMERA;
    Ray r = Ray(atmorg, viewdir);
    sphere1 = Sphere(vec3(0), planetradius + CloudsFloor);
    sphere2 = Sphere(vec3(0), planetradius + CloudsCeil);
    hitfloorX = rsi2(r, sphere1);
    vec2 data = texture(mainPassTex, viewdir).rg;
   // hash1x = rand2s(UV * vec2(Time, Time));
    vec3 hitman = CAMERA + viewdir * (data.g + hitfloorX);
    planet = Sphere(vec3(0), planetradius);
//    float mx1  = clamp(0.0, 1.0, MieScattCoeff * 0.2) * 0.5 + 0.5;
    weightshadow = 1001.0;
    float sun = getAO(hitman, 8.0 * (CloudsThresholdHigh - CloudsThresholdLow), 1001.0);// + (0.5 + max(0.0, (getAO(hitman, 1.0) * 2.0 - 1.0 ))) * 0.2 + getAO(hitman, 0.0);
    //weightshadow  = 0.000;//1 * CloudsDensityScale;
   // float sss =  getAO(hitman, 0.6, CloudsDensityScale  );
    return sun;
    //return sun * 0.8 + sss * 0.2;
}

#define intersects(a) (a >= 0.0)
vec2 raymarchCloudsRay(){
    vec3 viewdir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    vec3 atmorg = vec3(0,planetradius,0) + CAMERA;
    Ray r = Ray(atmorg, viewdir);
    float height = length(atmorg);
    float cloudslow = planetradius + CloudsFloor;
    float cloudshigh = planetradius + CloudsCeil;
    vec3 campos = CAMERA;


    sphere1 = Sphere(vec3(0), planetradius + CloudsFloor);
    sphere2 = Sphere(vec3(0), planetradius + CloudsCeil);

    float planethit = rsi2(r, planet);
    float hitfloor = rsi2(r, sphere1);
    float floorminhit = minhit;
    float floormaxhit = maxhit;
    float hitceil = rsi2(r, sphere2);
    float ceilminhit = minhit;
    float ceilmaxhit = maxhit;
    float dststart = 0.0;
    float dstend = 0.0;
    float coverageinv = 1.0;
    vec2 res = vec2(0);/*
    if(height < cloudslow){
        if(planethit < 0){
            res = internalmarchconservative(campos + viewdir * hitfloor, campos + viewdir * hitceil);
        }
    } else if(height >= cloudslow && height < cloudshigh){
        if(intersects(hitfloor)){
            res = internalmarchconservative(campos, campos + viewdir * floorminhit);
            if(!intersects(planethit)){
                vec2 r2 = internalmarchconservative(campos + viewdir * floormaxhit, campos + viewdir * ceilmaxhit);
                float r =1.0 - (1.0 - res.r) * (1.0 - r2.r);
                res = mix(r2, res, res.r);
                res.r = r;
            }
        } else {
            res = internalmarchconservative(campos, campos + viewdir * hitceil);
        }
    } else if(height > cloudshigh){
        if(!intersects(hitfloor) && !intersects(hitceil)){
            res = vec2(0);
        } else if(!intersects(hitfloor)){
            res = internalmarchconservative(campos + viewdir * minhit, campos + viewdir * maxhit);
        } else {
            res = internalmarchconservative(campos + viewdir * ceilminhit, campos + viewdir * floorminhit);
        }
    }
    */
    res = internalmarchconservative(campos + viewdir * hitfloor, campos + viewdir * hitceil);
    return res;
}
