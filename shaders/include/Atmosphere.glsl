
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
uniform vec3 SunDirection;
uniform float AtmosphereScale;
uniform float CloudsDensityScale;
uniform float CloudsDensityThresholdLow;
uniform float CloudsDensityThresholdHigh;
uniform float Time;
uniform float WaterWavesScale;
uniform float Rand1;
uniform float Rand2;
uniform float MieScattCoeff;

#export float FBMO1
#export float FBMO2
#export float FBMS1
#export float FBMS2
#export float WindBigScale
#export float WindSmallScale
#export float WindBigPower
#export float WindSmallPower
#export float GodRayPower
#export float FBMINITSCALE
#export float FBMINITSCALE2


layout(binding = 18) uniform samplerCube cloudsCloudsTex;
layout(binding = 19) uniform samplerCube atmScattTex;
layout(binding = 29) uniform samplerCube mainPassTex;
//layout(binding = 22) uniform sampler2D atmScattTex;
layout(binding = 20) uniform sampler2D cloudsRefShadowTex;

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
vec3 getWind(vec3 p){
    vec3 p2 = p * (noise3d(p) + 0.5);
    return vec3(
        noise3d(p2 + noise3d(p2 * 0.5) * 1.5),
        noise3d(-p2 + noise3d(p2 * 0.5) * 1.5),
        noise3d(p2.zxy + noise3d(p2 * 0.5) * 1.5)
    ) * 2.0 - 1.0;
}

#define xdnoise(a) ssnoise(a + ssnoise(a * 0.5) * 0.5)

float hcl = 0.0;
float cloudsDensity3DXXXX(vec3 pos){
    hcl = 1.0 - smoothstep(CloudsFloor, CloudsCeil, length(vec3(0, planetradius, 0) + pos) - planetradius);
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
} 
#define fbm fbm_alu
float fbm(vec3 p){
    p *= 0.001;
	float a = 0.0;
    float w = 1.0;
	for(int i=0;i<5;i++){
		a += noise3d(p) * w;	
        w *= 0.5;
		p = p * 4.0;
	}
	return a;
}
float cloudsDensity3D(vec3 pos){
    vec3 ps = pos +CloudsOffset * 111;// + wtim;   
    ps += (getWind(pos * 0.0005  * WindBigScale) * WindBigPower * 1.3 + getWind(pos * 0.00155  * WindSmallScale) * WindSmallPower * 0.1) * 2000.0 + CloudsOffset * 100.0;
    float density = 1.0 - fbm(ps * 0.05 + 0.5 * fbm(ps * 0.03 + Time * 0.01));
    float init = smoothstep(CloudsThresholdLow, CloudsThresholdHigh,  density);
    return  init;
}

float rand2s(vec2 co){
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
float rand2sTime(vec2 co){
    co *= Time;
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

Sphere sphere1;
Sphere sphere2;

float weightshadow = 2.0;
float internalmarchconservativeCoverageOnly(vec3 p1, vec3 p2){
    float iter = 0.0;
    const int stepcount = 5;
    const float stepsize = 1.0 / float(stepcount);
    float rd = rand2sTime(UV) * stepsize;
    float coverageinv = 1.0;
    float linear = distance(p1, mix(p1, p2, stepsize));
    for(int i=0;i<stepcount;i++){
        iter += rd;
        vec3 pos = mix(p1, p2, iter + rd);
        float clouds = cloudsDensity3D(pos);
        coverageinv -= clouds * linear * 0.001 * weightshadow * stepsize * CloudsDensityScale;
     //   if(coverageinv <= 0.0) break;
    }
    return  clamp(coverageinv, 0.0, 1.0);
}

float hash1x = UV.x * UV.y * Time;
vec3 randdir(){
    float x = rand2sTime(UV * hash1x);
    hash1x += 0.5451;
    float y = rand2sTime(UV * hash1x);
    hash1x += 0.62123;
    float z = rand2sTime(UV * hash1x);
    hash1x += 0.4652344;
    return (vec3(
        x, y, z
    ) * 2.0 - 1.0);
}

float intersectplanet(vec3 pos){
    Ray r = Ray(vec3(0,planetradius ,0) +pos, SunDirection);
    float hitceil = rsi2(r, planet);
    return max(0.0, -sign(hitceil));
}
float getAO(vec3 pos){
    vec3 dir = normalize(SunDirection + randdir() * 0.2);
    Ray r = Ray(vec3(0,planetradius ,0) +pos, dir);
    float hitceil = rsi2(r, sphere2);
    float hitfloor = rsi2(r, sphere1);
    //vec3 posceil = hitfloor < hitceil && hitfloor > 0.0 ? (pos + dir * min(hitfloor, planetradius * 0.01)) : (pos + dir * min(hitceil, planetradius * 0.01));
    vec3 posceil = pos + dir * min(hitceil, planetradius * 0.01);
    return internalmarchconservativeCoverageOnly(pos, posceil);
}
float getSUN(vec3 pos){
    vec3 dir = normalize(SunDirection);
    Ray r = Ray(vec3(0,planetradius ,0) +pos, dir);
    float hitceil = rsi2(r, sphere2);
    float hitfloor = rsi2(r, sphere1);
    //vec3 posceil = hitfloor < hitceil && hitfloor > 0.0 ? (pos + dir * min(hitfloor, planetradius * 0.01)) : (pos + dir * min(hitceil, planetradius * 0.01));
    vec3 posceil = pos + dir * min(hitceil, planetradius * 0.01);
    return internalmarchconservativeCoverageOnly(pos, posceil);
}
float getSunShadow(vec3 pos){
    float a = 0;
        vec3 dir = SunDirection;
        //vec3 dir = normalize(randdir());
        //dir.y = abs(dir.y);
       // vec3 dir = normalize(SunDirection);
        Ray r = Ray(vec3(0,planetradius ,0) +pos, dir);
        float hitceil = rsi2(r, sphere2);
        float hitceil2 = rsi2(r, sphere1);
        vec3 posceil = hitceil2 < hitceil && hitceil2 > 0.0 ? (pos + dir * min(hitceil2, planetradius * 0.01)) : (pos + dir * min(hitceil, planetradius * 0.01));
       // vec3 posceil =  (pos + dir * min(hitceil, planetradius * 0.01));
      //  float hitceil2 = min(rsi2(r, sphere1), 5000.0);
       // vec3 posceil2 = hitceil2 > 0.0 ? pos + dir * hitceil2 : posceil;
        a +=internalmarchconservativeCoverageOnly(pos, posceil);
    return a ;
}
float godray(vec3 pos){
    vec3 dir = SunDirection;
    Ray r = Ray(vec3(0,planetradius ,0) +pos, dir);
    float hitceil = rsi2(r, sphere2);
    vec3 posceil = pos + dir * hitceil;
    float hitfloor = rsi2(r, sphere1);
    vec3 posfloor = hitfloor > 0.0 ? (pos + dir * hitfloor) : pos;
    return internalmarchconservativeCoverageOnly(posfloor, posceil);
}

float godrayrandompoint(vec3 p1, vec3 p2){
    return godray(mix(p1, p2, fract(rand2sTime(UV) + hash(Time))));
}

vec2 internalmarchconservative(vec3 p1, vec3 p2){
    int stepcount = 5;
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
    float fulldistinv = distance(p1, p2) * stepsize;
    depr += distance(CameraPosition, lastpos);
    depw += 1.0;
    for(int i=0;i<stepcount;i++){
        pos = mix(p1, p2, iter + rd);
        clouds = cloudsDensity3D(pos);
        
        coverageinv -= clouds;
        depr += step(0.99, coverageinv) * distance(lastpos, pos);
        if(coverageinv <= 0.0) break;
        lastpos = pos;
        iter += stepsize;
        //rd = fract(rd + iter * 124.345345);
    }
    float cv = 1.0 - clamp(coverageinv, 0.0, 1.0);
    return vec2(cv, depr);
}

float shadows(){
    vec3 viewdir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    vec2 data = texture(mainPassTex, viewdir).rg;
    hash1x = rand2s(UV * vec2(Time, Time));
    vec3 hitman = viewdir * data.g;
    sphere1 = Sphere(vec3(0), planetradius + CloudsFloor);
    sphere2 = Sphere(vec3(0), planetradius + CloudsCeil);
    return data.r < 0.001 ? 1.0 : (getAO(hitman));
}
float skyfog(){
    if(NoiseOctave1 <= 0.1) return 0.0;
    vec3 viewdir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    vec2 data = texture(mainPassTex, viewdir).rg;
    vec3 hitman = viewdir * data.g;
    float iter = 0.0;
    weightshadow = 100.0;
    int stepcount = 7;
    float stepsize = 1.0 / float(stepcount);
    float rd = fract(rand2sTime(UV) + hash(Time)) * stepsize;
    float godr = 0.0;
    vec3 pos = CameraPosition;
    //float godrcoverage = 1.0;
    float fulldistinv = distance(CameraPosition, hitman) * stepsize;
    sphere1 = Sphere(vec3(0), planetradius + CloudsFloor);
    sphere2 = Sphere(vec3(0), planetradius + CloudsCeil);
    for(int i=0;i<stepcount;i++){
        pos = mix(CameraPosition, hitman, rd + iter);
        iter += stepsize;
        godr += godray(pos)  * NoiseOctave1;
    }
    return pow(godr * 0.025, 4.0);
}

#define intersects(a) (a >= 0.0)
vec2 raymarchCloudsRay(){
    vec3 viewdir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    vec3 atmorg = vec3(0,planetradius,0) + CameraPosition;  
    float height = length(atmorg);
    vec3 campos = CameraPosition;
    float cloudslow = planetradius + CloudsFloor;
    float cloudshigh = planetradius + CloudsCeil;
    
    Ray r = Ray(atmorg, viewdir);
    
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
    vec2 res = vec2(0);
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
    
    return res;
}
