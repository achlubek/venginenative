
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

#define iSteps 8
#define jSteps 6

struct Ray { vec3 o; vec3 d; };
struct Sphere { vec3 pos; float rad; };

bool shouldBreak(){
   vec2 position = UV * 2.0 - 1.0;
    if(length(position)> 1.0) return true; 
    else return false;    
}

float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{ return dot(point - origin, normal) / dot(direction, normal); }


float planetradius = 6371e3;
Sphere planet = Sphere(vec3(0), planetradius);

float minhit = 0.0;
float maxhit = 0.0;
float rsi2(in Ray ray, in Sphere sphere)
{
    vec3 oc = ray.o - sphere.pos;
    float b = 2.0 * dot(ray.d, oc);
    float c = dot(oc, oc) - sphere.rad*sphere.rad;
    float disc = b * b - 4.0 * c;
    if (disc < 0.0) return -1.0;
    float q = b < 0.0 ? ((-b - sqrt(disc))/2.0) : ((-b + sqrt(disc))/2.0);
    float t0 = q;
    float t1 = c / q;
    if (t0 > t1) {
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }
    minhit = min(t0, t1);
    maxhit = max(t0, t1);
    if (t1 < 0.0) return -1.0;
    if (t0 < 0.0) return t1;
    else return t0; 
}


vec3 atmosphere(vec3 r, vec3 r0, vec3 pSun, float iSun, float rPlanet, float rAtmos, vec3 kRlh, float kMie, float shRlh, float shMie, float g) {
    pSun = normalize(pSun);
    r = normalize(r);
    float iStepSize = rsi2(Ray(r0, r), Sphere(vec3(0), rAtmos)) / float(iSteps);
    float iTime = 0.0;
    vec3 totalRlh = vec3(0,0,0);
    vec3 totalMie = vec3(0,0,0);
    float iOdRlh = 0.0;
    float iOdMie = 0.0;
    float mu = dot(r, pSun);
    float mumu = mu * mu;
    float gg = g * g;
    float pRlh = 3.0 / (16.0 * PI) * (1.0 + mumu);
    float pMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));
    for (int i = 0; i < iSteps; i++) {
        vec3 iPos = r0 + r * (iTime + iStepSize * 0.5);
        float iHeight = length(iPos) - rPlanet;
        float odStepRlh = exp(-iHeight / shRlh) * iStepSize;
        float odStepMie = exp(-iHeight / shMie) * iStepSize;
        iOdRlh += odStepRlh;
        iOdMie += odStepMie;
        float jStepSize = rsi2(Ray(iPos, pSun), Sphere(vec3(0),rAtmos)) / float(jSteps);
        float jTime = 0.0;
        float jOdRlh = 0.0;
        float jOdMie = 0.0;
        float invshRlh = 1.0 / shRlh;
        float invshMie = 1.0 / shMie;
        for (int j = 0; j < jSteps; j++) {
            vec3 jPos = iPos + pSun * (jTime + jStepSize * 0.5);
            float jHeight = length(jPos) - rPlanet;
            jOdRlh += exp(-jHeight * invshRlh) * jStepSize;
            jOdMie += exp(-jHeight * invshMie) * jStepSize;
            jTime += jStepSize;
        }
        vec3 attn = exp(-(kMie * (iOdMie + jOdMie) + kRlh * (iOdRlh + jOdRlh)));
        totalRlh += odStepRlh * attn;
        totalMie += odStepMie * attn;
        iTime += iStepSize;
    }   
    return max(vec3(0.0), iSun * (pRlh * kRlh * totalRlh + pMie * kMie * totalMie));
}

vec3 sun(vec3 dir, vec3 sundir, float gloss, float ansio){
    float dt = max(0, dot(normalize(mix(dir, dir + vec3(dir.x, sundir.y, dir.z), ansio)), sundir));
    return pow(dt*dt*dt*dt*dt, clamp(256.0 * gloss , 44.0, 412.0)) * vec3(64.0 * gloss * gloss);
   // return smoothstep(0.997, 1.0, dt) * vec3(122);
}

vec3 getAtmosphereForDirectionRealX(vec3 origin, vec3 dir, vec3 sunpos){
    return atmosphere(
        dir,           // normalized ray direction
        vec3(0,planetradius  ,0)+ origin,               // ray origin
        sunpos,                        // position of the sun
        64.0,                           // intensity of the sun
        planetradius,                         // radius of the planet in meters
        6471e3,                         // radius of the atmosphere in meters
        vec3(5.5e-6, 13.0e-6, 22.4e-6), // Rayleigh scattering coefficient
        21e-6,                          // Mie scattering coefficient
        5e3,                            // Rayleigh scale height
        1.2e3 * MieScattCoeff,                          // Mie scale height
        0.1758                           // Mie preferred scattering direction
    );
}
vec3 getAtmosphereForDirection(vec3 dir, float roughness){
    float levels = max(0, float(textureQueryLevels(atmScattTex)));
    float mx = log2(roughness*1024.0+1.0)/log2(1024.0);
    float mlvel = mx * levels;
    return textureLod(atmScattTex, dir, mlvel).rgb;
}

float hash( float n ){
    return fract(sin(n)*758.5453);
}

float noise( in vec3 x ){
    vec3 p = floor(x);
    vec3 f = fract(x); 
    float n = p.x + p.y*57.0 + p.z*800.0;
    float res = mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x), mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
    mix(mix( hash(n+800.0), hash(n+801.0),f.x), mix( hash(n+857.0), hash(n+858.0),f.x),f.y),f.z);
    return res;
}
#define ssnoise(a) (snoise(a) * 0.5 + 0.5)
/*
#define fbmsamples 4
#define fbm fbm_alu_optimized
//#define ssnoise(a) (noise(a))
float fbm_alu(vec3 p){
    //p *= 0.2;
	float a = 0.0;
    float w = NoiseOctave6 * 0.1;
    float sum = 0.0;
    //p += ssnoise(p);	
    //w *= 0.7;
   // p = p * 4.0;
    vec3 px = p;
    px *= 0.5;
    
    a += ssnoise(px * NoiseOctave5) * w;	
    sum += w;
    
    px = p * 2.0;
    w = 0.05;
    a += noise(px + noise(px * 0.1)) * w;	
    sum += w;
    
    px = px * 4.0;
    w = 0.02;
    a += noise(px + noise(px * 3.0)) * w;	
    sum += w;
    
    px = px * 4.0;
    w = 0.005;
    a += noise(px + noise(px * 3.0)) * w;	
    sum += w;
    
	return a / sum;
}
float fbm_aluX(vec3 p){
    p *= 0.1;
   // p.xz *= NoiseOctave8;
	float a = 0.0;
    float w = 1.0;
	for(int i=0;i<fbmsamples;i++){
		a += ssnoise(p) * w;	
        w *= NoiseOctave8;
		p *= NoiseOctave6;
	}
	return a;
}

float fbm_alu_optimized(vec3 p){
    p *= 0.1 * FBMINITSCALE;
    return (ssnoise(p * FBMO1) * FBMS1+
     noise(p * FBMO1 * FBMO2) * FBMS2 +
     noise(p * FBMO1 * FBMO2 * FBMO3) * FBMS3 +
     noise(p * FBMO1 * FBMO2 * FBMO3 * FBMO4) * FBMS4) / (FBMS1 + FBMS2 + FBMS3 + FBMS4);
}*/
vec3 getWind(vec3 p){
    vec3 p2 = p * (noise(p) + 0.5);
    return vec3(
        noise(p2 + noise(p2 * 0.5) * 1.5),
        noise(-p2 + noise(p2 * 0.5) * 1.5),
        noise(p2.zxy + noise(p2 * 0.5) * 1.5)
    ) * 2.0 - 1.0;
}
/*
float dividerrcp = 1.0 / (CloudsCeil - CloudsFloor);
vec3 wind = Time * vec3(0.0, 0.0, 0.01  );
float edgeclose = 0.0;
float cloudsDensity3DFBMReal(vec3 pos){
    vec3 ps = pos +CloudsOffset * 100.0;
    float h =  (length(vec3(0, planetradius, 0) + pos) - planetradius - CloudsFloor) * dividerrcp; 
    float hw = smoothstep(0.0, 0.08, h) * (1.0 - smoothstep(0.7, 1.0, h));
    //ps.xz *= CloudsDensityScale;
   // float density = 1.0 - fbm(ps * 0.05 + fbm(ps * 0.5));
    //float density = 1.0 - fbm(ps * 0.05);
    float density = 1.0 - fbm(ps * 0.0005 + getWind(ps * 0.0005  * WindBigScale) * WindBigPower * 0.3 + getWind(ps * 0.00055  * WindSmallScale) * WindSmallPower * 0.1);
    
    float init = smoothstep(CloudsThresholdLow, CloudsThresholdHigh,  density);
    //edgeclose = pow(1.0 - abs(CloudsThresholdLow - density), CloudsThresholdHigh * 113.0);
    float mid = (CloudsThresholdLow + CloudsThresholdHigh) * 0.5;
    edgeclose = pow(1.0 - abs(mid - density) * 2.0, 1.0);
    return init ;
} 
*/
#define xdnoise(a) ssnoise(a + ssnoise(a * 0.5) * 0.5)
float hcl = 0.0;
float cloudsDensity3D(vec3 pos){
    hcl = 1.0 - smoothstep(CloudsFloor, CloudsCeil, length(vec3(0, planetradius, 0) + pos) - planetradius);
    pos += (getWind(pos * 0.0005  * WindBigScale) * WindBigPower * 0.3 + getWind(pos * 0.00155  * WindSmallScale) * WindSmallPower * 0.1) * 2000.0 + CloudsOffset * 100.0;
    float partitions = xdnoise(pos * 0.00001 * vec3(FBMINITSCALE, FBMINITSCALE, FBMINITSCALE));
    float partitions2 = xdnoise(pos * 0.00004 * vec3(FBMINITSCALE2, FBMINITSCALE2, FBMINITSCALE2));
    float aza = smoothstep(0.0, 0.1, hcl) * (1.0 - smoothstep(0.8, 1.0, hcl));
    partitions = (partitions + partitions2) * 0.5;
    float fao1 = FBMO1 * 0.1;
    float fao2 = FBMO2 * 0.1;
    //float localaberations = mix(1.0, xdnoise(pos * 0.0001 * FBMS1) * fao1 + 1.0 * (1.0 - fao1), hcl);
    float localaberations = xdnoise(pos * 0.0001 * FBMS1) * fao1 + 1.0 * (1.0 - fao1);
    //float localaberations2 = mx(1.0, xdnoise(pos * 0.0001 * FBMS1) * fao1 + 1.0 * (1.0 - fao1), hcl);
    float localaberations2 = xdnoise(pos * 0.001 * FBMS2).x * fao2 + 1.0 * (1.0 - fao2);
    float density = partitions * localaberations * localaberations2;
    return smoothstep(
        CloudsThresholdLow,
        CloudsThresholdHigh,
        density * aza);
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

float weightshadow = 1.0;
float internalmarchconservativeCoverageOnly(vec3 p1, vec3 p2){
    float iter = 0.0;
    const int stepcount = 5;
    const float stepsize = 1.0 / float(stepcount);
    float rd = rand2sTime(UV) * stepsize;
    float coverageinv = 1.0;
    float linear = distance(p1, mix(p1, p2, stepsize));
    for(int i=0;i<stepcount;i++){
        iter += stepsize;
        vec3 pos = mix(p1, p2, iter + rd);
        float clouds = cloudsDensity3D(pos);
        coverageinv -= clouds * weightshadow * stepsize * CloudsDensityScale;
        if(coverageinv <= 0.0) break;
    }
    return  clamp(coverageinv, 0.0, 1.0);
}

float hash1x = 0.0;
vec3 randdir(){
    float x = rand2sTime(UV * hash1x);
    hash1x += 2.5451;
    float y = rand2sTime(UV * hash1x);
    hash1x += 3.62123;
    float z = rand2sTime(UV * hash1x);
    hash1x += 2.4652344;
    return (vec3(
        x, y, z
    ) * 2.0 - 1.0);
}

float intersectplanet(vec3 pos){
    Ray r = Ray(vec3(0,planetradius ,0) +pos, normalize(SunDirection));
    float hitceil = rsi2(r, planet);
    return max(0.0, -sign(hitceil));
}
float getAO(vec3 pos){
    vec3 dir = normalize(normalize(SunDirection) + randdir() * 2.03);
    Ray r = Ray(vec3(0,planetradius ,0) +pos, dir);
    float hitceil = rsi2(r, sphere2);
    float hitfloor = rsi2(r, sphere1);
    //vec3 posceil = hitfloor < hitceil && hitfloor > 0.0 ? (pos + dir * min(hitfloor, planetradius * 0.01)) : (pos + dir * min(hitceil, planetradius * 0.01));
    vec3 posceil = pos + dir * min(hitceil, planetradius * 0.01);
    return internalmarchconservativeCoverageOnly(pos, posceil);
}
float getSUN(vec3 pos){
    vec3 dir = normalize(normalize(SunDirection) + randdir() * 0.7);
    Ray r = Ray(vec3(0,planetradius ,0) +pos, dir);
    float hitceil = rsi2(r, sphere2);
    float hitfloor = rsi2(r, sphere1);
    //vec3 posceil = hitfloor < hitceil && hitfloor > 0.0 ? (pos + dir * min(hitfloor, planetradius * 0.01)) : (pos + dir * min(hitceil, planetradius * 0.01));
    vec3 posceil = pos + dir * min(hitceil, planetradius * 0.01);
    return internalmarchconservativeCoverageOnly(pos, posceil);
}
float getSunShadow(vec3 pos){
    float a = 0;
        vec3 dir = normalize(normalize(SunDirection));
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
    vec3 dir = normalize(SunDirection);
    Ray r = Ray(vec3(0,planetradius ,0) +pos, dir);
    float hitceil = rsi2(r, sphere2);
    vec3 posceil = pos + dir * hitceil;
    float hitfloor = rsi2(r, sphere1);
    vec3 posceil2 = hitfloor > 0.0 ? pos + dir * hitfloor : pos;
    return internalmarchconservativeCoverageOnly(posceil2, posceil);
}

float godrayrandompoint(vec3 p1, vec3 p2){
    return godray(mix(p1, p2, fract(rand2sTime(UV) + hash(Time))));
}

vec2 internalmarchconservative(vec3 p1, vec3 p2){
    int stepcount = 5;
    float stepsize = 1.0 / float(stepcount);
    float rd = fract(rand2sTime(UV) + hash(Time)) * stepsize;
    hash1x = rand2s(UV * vec2(Time, Time));
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
        pos = mix(p1, p2, rd + iter);
        clouds = cloudsDensity3D(pos);// * (1.0 - rd);
       // pos = mix(vec3(0,1,0), p2, iter + rd);
      //  c += edgeclose * getAOPos(scale, pos);
      //  w += edgeclose;
       // if(coverageinv > 0.0 && clouds > 0.0){
       //     c += coverageinv * getAO(pos);
       //     w += coverageinv;
       // }
       // godr += coverageinv * godrayrandompoint(CameraPosition, pos);
       // godw += coverageinv;
        //depw += coverageinv;
        coverageinv -= clouds;
        depr += step(0.99, coverageinv) * distance(lastpos, pos);
        if(coverageinv <= 0.0) break;
        lastpos = pos;
        iter += stepsize;
        //rd = fract(rd + iter * 124.345345);
    }
    return vec2(1.0 - clamp(coverageinv, 0.0, 1.0), depr);
}

float shadows(){
    vec3 viewdir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    vec2 data = texture(mainPassTex, viewdir).rg;
    vec3 hitman = viewdir * data.g;
    sphere1 = Sphere(vec3(0), planetradius + CloudsFloor);
    sphere2 = Sphere(vec3(0), planetradius + CloudsCeil);
    return data.r < 0.001 ? 1.0 : ((getAO(hitman) + getSUN(hitman)) * 0.5);
}
float skyfog(){
    if(NoiseOctave1 <= 0.1) return 0.0;
    vec3 viewdir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    vec2 data = texture(mainPassTex, viewdir).rg;
    vec3 hitman = viewdir * data.g;
    float iter = 0.0;
    int stepcount = 14;
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
        godr += godray(pos) * fulldistinv * 0.0001 * NoiseOctave1;
    }
    return godr * 0.05;
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
