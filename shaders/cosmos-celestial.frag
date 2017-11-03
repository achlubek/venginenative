#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform UniformBufferObject1 {
    float Time;
    float Zero;
    vec2 Mouse;
    mat4 VPMatrix;
    vec4 inCameraPos;
    vec4 inFrustumConeLeftBottom;
    vec4 inFrustumConeBottomLeftToBottomRight;
    vec4 inFrustumConeBottomLeftToTopLeft;
    vec2 Resolution;
} hiFreq;

struct GeneratedStarInfo {
    vec4 position_radius;
    vec4 color_age; //0->maybe 10? maybe 100?
    vec4 orbitPlane_rotationSpeed;
    vec4 spotsIntensity_zero_zero_zero; //0->1
};

struct GeneratedPlanetInfo {
    vec4 position_radius;
    vec4 terrainMaxLevel_fluidMaxLevel_starDistance_seed;
    vec4 habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength;
    vec4 preferredColor_zero;//0->1
    vec4 atmosphereAbsorbColor_zero;//0->1
    ivec4 hoststarindex_zero_zero_zero;
};

struct GeneratedMoonInfo {
    vec4 position_radius;
    vec4 orbitPlane_orbitSpeed;
    vec4 preferredColor_terrainMaxLevel;//0->1
    vec4 planetDistance_zero_zero_zero; //0->1
    ivec4 hostplanetindex_zero_zero_zero;
};

layout(set = 0, binding = 1) buffer StarsStorageBuffer {
    ivec4 count;
    GeneratedStarInfo stars[];
} starsBuffer;

layout(set = 0, binding = 2) buffer PlanetsStorageBuffer {
    ivec4 count;
    GeneratedPlanetInfo planets[];
} planetsBuffer;

layout(set = 0, binding = 3) buffer MoonsStorageBuffer {
    ivec4 count;
    GeneratedMoonInfo moons[];
} moonsBuffer;

float Time = hiFreq.Time;

vec3 CameraPosition = hiFreq.inCameraPos.xyz;
vec3 FrustumConeLeftBottom = hiFreq.inFrustumConeLeftBottom.xyz;
vec3 FrustumConeBottomLeftToBottomRight = hiFreq.inFrustumConeBottomLeftToBottomRight.xyz;
vec3 FrustumConeBottomLeftToTopLeft = hiFreq.inFrustumConeBottomLeftToTopLeft.xyz;

#include camera.glsl

struct Ray { vec3 o; vec3 d; };
struct Sphere { vec3 pos; float rad; };
float rsi2_simple(in Ray ray, in Sphere sphere)
{
    vec3 oc = ray.o - sphere.pos;
    float b = 2.0 * dot(ray.d, oc);
    return -b - sqrt(b * b - 4.0 * (dot(oc, oc) - sphere.rad*sphere.rad));
}
vec2 rsi2(in Ray ray, in Sphere sphere)
{
    vec3 oc = ray.o - sphere.pos;
    float b = 2.0 * dot(ray.d, oc);
    float c = dot(oc, oc) - sphere.rad*sphere.rad;
    float disc = b * b - 4.0 * c;
    vec2 ex = vec2(-b - sqrt(disc), -b + sqrt(disc))/2.0;
    return vec2(min(ex.x, ex.y), max(ex.x, ex.y));
}
mat3 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat3(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s,
        oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s,
        oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c);
}
float hash( float n ){
    return fract(sin(n)*758.5453);
}
float noise2d( in vec2 x ){
    vec2 p = floor(x);
    vec2 f = smoothstep(0.0, 1.0, fract(x));
    float n = p.x + p.y*57.0;
    return mix(
        mix(hash(n+0.0),hash(n+1.0),f.x),
        mix(hash(n+57.0),hash(n+58.0),f.x),
        f.y
       );
}
float noise3d( in vec3 x ){
    vec3 p = floor(x);
        vec3 f = smoothstep(0.0, 1.0, fract(x));
    float n = p.x + p.y*157.0 + 113.0*p.z;

    return mix(mix(	mix( hash(n+0.0), hash(n+1.0),f.x),
            mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
           mix(	mix( hash(n+113.0), hash(n+114.0),f.x),
            mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}
// YOU ARE WELCOME! 4d NOISE
float noise4d(vec4 x){
    vec4 p=floor(x);
    vec4 f=smoothstep(0.,1.,fract(x));
    float n=p.x+p.y*157.+p.z*113.+p.w*971.;
    return mix(mix(mix(mix(hash(n),hash(n+1.),f.x),mix(hash(n+157.),hash(n+158.),f.x),f.y),
    mix(mix(hash(n+113.),hash(n+114.),f.x),mix(hash(n+270.),hash(n+271.),f.x),f.y),f.z),
    mix(mix(mix(hash(n+971.),hash(n+972.),f.x),mix(hash(n+1128.),hash(n+1129.),f.x),f.y),
    mix(mix(hash(n+1084.),hash(n+1085.),f.x),mix(hash(n+1241.),hash(n+1242.),f.x),f.y),f.z),f.w);
}
float FBM2(vec2 p, int octaves, float dx){
    float a = 0.0;
        float w = 0.5;
    for(int i=0;i<octaves;i++){
        a += noise2d(p) * w;
            w *= 0.5;
        p *= dx;
    }
    return a;
}
float FBM3(vec3 p, int octaves, float dx, float ww){
    float a = 0.0;
        float w = 0.5;
        float sw = 0.0;
    for(int i=0;i<octaves;i++){
        a += noise3d(p) * w;
            w *= ww;
        p *= dx;
    }
    return a;
}
float FBM4(vec4 p, int octaves, float dx){
    float a = 0.0;
        float w = 0.5;
    for(int i=0;i<octaves;i++){
        a += noise4d(p) * w;
            w *= 0.5;
        p *= dx;
    }
    return a;
}

float fbmHI(vec3 p, float dx){
   // p *= 0.1;
    p *= 1.2;
    //p += getWind(p * 0.2) * 6.0;
    float a = 0.0;
    float w = 1.0;
    float wc = 0.0;
    for(int i=0;i<5;i++){
        //p += noise(vec3(a));
        a += clamp(2.0 * abs(0.5 - (noise3d(p))) * w, 0.0, 1.0);
        wc += w;
        w *= 0.5;
        p = p * dx;
    }
    return a / wc;// + noise(p * 100.0) * 11;
}
float rand2s(vec2 co){
    return fract(sin(dot(co.xy * Time,vec2(12.9898,78.233))) * 43758.5453);
}

float infinity = 1.0 / 0.0;
Ray cameraRay;
#define hits(a) (a > 0.0 && a < infinity)

// BEGIN MOON RENDERING
GeneratedMoonInfo currentMoon;
GeneratedStarInfo moonHostStar;
GeneratedPlanetInfo moonHostPlanet;

// END MOON RENDERING

struct Fragment{
    float depth;
    float alpha;
    vec3 color;
};

Fragment fragments[9] = Fragment[9](
    Fragment(0,0,vec3(0.0)),
    Fragment(0,0,vec3(0.0)),
    Fragment(0,0,vec3(0.0)),
    Fragment(0,0,vec3(0.0)),
    Fragment(0,0,vec3(0.0)),
    Fragment(0,0,vec3(0.0)),
    Fragment(0,0,vec3(0.0)),
    Fragment(0,0,vec3(0.0)),
    Fragment(0,0,vec3(0.0))
);
int fragmentIndex = 0;

void storeFragment(float depth, float alpha, vec3 color){
    if(fragmentIndex < 9){
        fragments[fragmentIndex++] = Fragment(depth, alpha, color);
    }
}

Fragment findMaximum(float limit){
    Fragment fragment = Fragment(0.0, 0, vec3(0.0));
    for(int i=0;i<fragmentIndex;i++){
        if(fragments[i].depth > fragment.depth && fragments[i].depth < limit){
            fragment = fragments[i];
        }
    }
    return fragment;
}

vec4 resolveFragments(){
    vec4 result = vec4(0.0);
    float lastmin = 99999999.0;
    for(int i=0;i<fragmentIndex;i++){
        Fragment f = findMaximum(lastmin);
        lastmin = f.depth - 0.0001;
        result.xyz = mix(result.xyz, f.color, f.alpha);
        result.a = max(result.a, f.alpha);
    }
    return result;
}


#define MSUNDIR (moonHostStar.position_radius.rgb - CameraPosition)
float getmoonheight(vec3 dir){
    return currentMoon.position_radius.a
        + (1.0 - 2.0 * abs(0.5 - FBM3(dir * 10.0, 6, 2.1, 0.5)))
        * currentMoon.preferredColor_terrainMaxLevel.a;
}
vec3 getmoonnormal(vec3 dir){
    vec3 tangdir = normalize(cross(dir, vec3(0.0, 1.0, 0.0)));
    vec3 bitangdir = normalize(cross(tangdir, dir));
    mat3 normrotmat1 = rotationMatrix(tangdir, 0.002);
    mat3 normrotmat2 = rotationMatrix(bitangdir, 0.002);
    vec3 dir2 = normrotmat1 * dir;
    vec3 dir3 = normrotmat2 * dir;
    vec3 p1 = dir * getmoonheight(dir);
    vec3 p2 = dir2 * getmoonheight(dir2);
    vec3 p3 = dir3 * getmoonheight(dir3);
    return normalize(cross(normalize(p3 - p1), normalize(p2 - p1)));
}
vec4 traceMoon(Ray ray){
    Sphere surface = Sphere(currentMoon.position_radius.rgb, currentMoon.position_radius.a);
    vec3 color = vec3(0.0);
    float coverage = 0.0;
    float hit_Surface = rsi2(ray, surface).x;

    if(!hits(hit_Surface) ) return vec4(0.0, 0.0, 0.0, -1.0);
    vec3 norm = getmoonnormal(normalize((ray.d * hit_Surface) - currentMoon.position_radius.rgb));
    float plhei = getmoonheight(normalize((ray.d * hit_Surface) - currentMoon.position_radius.rgb));
    float plheidelta = (plhei - currentMoon.position_radius.a) / currentMoon.preferredColor_terrainMaxLevel.a;
    vec3 newdir = plhei * normalize((ray.d * hit_Surface) - currentMoon.position_radius.rgb);
    vec3 sundir = normalize(MSUNDIR - ray.d * hit_Surface);
    float shadow = smoothstep(-0.2 * plheidelta, 0.2, dot(sundir, newdir));
    color = shadow * vec3(currentMoon.preferredColor_terrainMaxLevel.rgb) * max(0.0, dot(norm, normalize(MSUNDIR - (ray.d * hit_Surface))));
    coverage = 1.0;

    return vec4(color, coverage);
}
// BEGIN PLANET RENDERING

GeneratedPlanetInfo currentPlanet;
GeneratedStarInfo planetHostStar;
#define SUNDIR (planetHostStar.position_radius.rgb - CameraPosition)
vec3 extra_cheap_atmosphere(float raylen, float sunraylen, float absorbstrength, vec3 absorbcolor, float sunraydot){
    //sundir.y = max(sundir.y, -0.07);
    sunraydot = max(0.0, sunraydot);
    raylen *= absorbstrength * 0.004;
    sunraylen *= absorbstrength * 0.004;
    sunraylen = min(sunraylen, 1.8);
    float raysundt = pow(abs(sunraydot), 2.0);
    float sundt = pow(max(0.0, sunraydot), 8.0);
    float mymie = sundt * raylen;
    vec3 suncolor = mix(vec3(1.0), max(vec3(0.0), vec3(1.0) - absorbcolor), sunraylen);// / (1.0 + raylen);
    vec3 bluesky= absorbcolor * suncolor;
    vec3 bluesky2 = max(vec3(0.0), bluesky - absorbcolor * 0.08 * (raylen));
    bluesky2 *= raylen * (0.24 + raysundt * 0.24);
    return bluesky2 + mymie * suncolor;
}
#line 231

float clouds(vec3 mx, float h){
    return pow(smoothstep(0.136 - noise3d(mx * 1.3) * 0.03, 0.16, noise3d(mx * 2.0) * h * fbmHI(mx * 6.0 + 5.0*fbmHI(mx.yxz * 1.0 + hiFreq.Time * 0.001, 2.0) * 0.5 - hiFreq.Time * 0.01, 2.8)), 3.0);
}

float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{ return dot(point - origin, normal) / dot(direction, normal); }

vec4 traceRings(Ray ray, Sphere surface, vec3 sundir){
    float plane = intersectPlane(ray.o, ray.d, currentPlanet.position_radius.xyz, normalize(vec3(sin(hiFreq.Time * 0.01), 0.0, cos(hiFreq.Time * 0.01))));
    vec3 color = vec3(0.0);
    float coverage = 0.0;
    float hit_Surface = rsi2(ray, surface).x;
    if(plane > 0.0){
        vec3 pos = ray.o + ray.d * plane;
        float planetdist = distance(currentPlanet.position_radius.xyz, pos);
        float r1 = hash(currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.a);
        float r2 = hash(currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.a + 100.0);
        float r3 = hash(currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.a + 200.0);
        float r4 = hash(currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.a + 400.0);
        float multprimary = smoothstep(currentPlanet.position_radius.a * 2.0, currentPlanet.position_radius.a * (3.0 + r1 * 2.0), planetdist);
        float multsecondary = 1.0 - smoothstep(currentPlanet.position_radius.a *  (6.0 + r2 * 2.0), currentPlanet.position_radius.a*  (9.0 + r3 * 2.0), planetdist);
        float multflunc = smoothstep(0.5, 0.8, noise2d(vec2(planetdist * 0.015 * (0.8 + 0.4 * r4), 0.0)));
        float hit_Surface2 = hits(rsi2(Ray(pos, sundir), surface).x) ? 0.0 : 1.0;
        color += hit_Surface2 * multprimary * multsecondary * currentPlanet.preferredColor_zero.rgb;
        coverage += multprimary * multsecondary * multflunc;
    }
    storeFragment(plane, coverage, color);
    return vec4(color, coverage);
}
vec4 tracePlanetAtmosphere(vec3 start, vec3 end, float lengthstart, float lengthstop){
    float seed = currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.a;
    float rd1 = hash(seed);
    float rd2 = hash(seed + 200.0);
    float rd3 = hash(seed + 400.0);
    vec4 result = vec4(0.0);
//    float raylen = distance(start, end);
    Sphere surface = Sphere(currentPlanet.position_radius.rgb, currentPlanet.position_radius.a);
    Sphere atmosphere = Sphere(currentPlanet.position_radius.rgb,
        currentPlanet.position_radius.a + currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b);

    vec3 atm = vec3(0.0);
    float stepdistance = 0.7;
    float iter = rand2s(UV) * stepdistance + 0.001;
    float coverage = 0.0;
    float dstnew = 0.0;
    float dsttrg = distance(start, end);
    vec3 drr = normalize(end - start);
    while(dstnew < dsttrg){
        vec3 p = start + drr * iter;
        dstnew = distance(start, p);
        vec3 sundir = normalize(SUNDIR - p);
        vec2 hit_Atmosphere = rsi2(Ray(p, sundir), atmosphere);
        vec2 hit_Surface = rsi2(Ray(p, sundir), surface);
        iter += stepdistance;
        float surfacemulti = 1.0;
        if(hits(max(hit_Surface.x, hit_Surface.y))) surfacemulti = 0.0;
        float heightmix = smoothstep(lengthstart, lengthstop, distance(currentPlanet.position_radius.rgb, p));
        vec3 dir = normalize(p - currentPlanet.position_radius.rgb);
        float dst = length(p - currentPlanet.position_radius.rgb);
        vec4 coorddir = vec4(dir * (1.0 + 0.1 * heightmix) * 11.0 * (0.2 + rd2 * 3.0), hiFreq.Time * 0.001);
        //float cloudiness = clouds(dir ,  1.0 - abs( heightmix * 2.0 - 1.0)  );
        float flunctuations = FBM3(dir * vec3(1.0, 10.0, 1.0), 4, 3.0, 0.5);
        float shadow = 1.0;// - traceRings(Ray(p, sundir), surface, sundir).a;
        float absorbstrength = currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.a * (0.1 + 0.9 * flunctuations);
        vec3 AC =  extra_cheap_atmosphere(100.0 * stepdistance, clamp(max(hit_Atmosphere.x, hit_Atmosphere.y), 0.0, 10000.0), 1.0
                * absorbstrength,
                currentPlanet.atmosphereAbsorbColor_zero.rgb *  (0.1 + 0.9 * flunctuations),
                dot(drr, sundir));
        atm += clamp(surfacemulti * shadow * vec3(shadow) * 10.0 * stepdistance * clamp(1.0 - coverage, 0.0, 1.0) *  (1.0 - step(0.0, max(hit_Surface.x, hit_Surface.y))) * (AC ) * (1.0 - 0.04 * clamp(max(hit_Atmosphere.x, hit_Atmosphere.y), 0.0, 10000.0) * absorbstrength), 0.0,1.0);
        coverage += stepdistance * absorbstrength * 1.5;
        if(coverage > 1.0) break;
    }
    //atm *= 0.1;
    return vec4(atm, clamp(coverage, 0.0, 1.0));
}
#define maxheight (0.005 * currentPlanet.position_radius.a)*currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.r*currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b
float getplanetheight(vec3 dir){
    float seed = currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.a;
    float rd1 = hash(seed);
    float rd2 = hash(seed + 200.0);
    float rd3 = hash(seed + 400.0);
    return currentPlanet.position_radius.a
        - (0.005 * currentPlanet.position_radius.a) * (1.0 - 2.0 * abs(0.5 - FBM3(dir * 1.0 * (0.2 + rd2 * 3.0), 8, 1.1 + rd1 * 1.6, rd3* 0.2 + 0.4 )))
        * currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.r
        * currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b;
}
vec3 getplanetnormal(vec3 dir){
    vec3 tangdir = normalize(cross(dir, vec3(0.0, 1.0, 0.0)));
    vec3 bitangdir = normalize(cross(tangdir, dir));
    mat3 normrotmat1 = rotationMatrix(tangdir, 0.002);
    mat3 normrotmat2 = rotationMatrix(bitangdir, 0.002);
    vec3 dir2 = normrotmat1 * dir;
    vec3 dir3 = normrotmat2 * dir;
    vec3 p1 = dir * getplanetheight(dir);
    vec3 p2 = dir2 * getplanetheight(dir2);
    vec3 p3 = dir3 * getplanetheight(dir3);
    return normalize(cross(normalize(p3 - p1), normalize(p2 - p1)));
}

float raymarchTerrain(Ray ray, vec3 center){
    float dist = 0.0;
    for(int i=0;i<300;i++){
        vec3 p = ray.o + ray.d * dist;
        vec3 dir = normalize(p - center);
        float ds = distance(p, center) - getplanetheight(dir);
        if(ds < 0.1) return dist;
        dist += ds * 0.8;
    }
    return -0.01;
}
float raymarchTerrainShadow(Ray ray, vec3 center){
    float dist = rand2s(UV) * 0.2 + 0.07;
    float s = 0.0;
    for(int i=0;i<6;i++){
        vec3 p = ray.o + ray.d * currentPlanet.position_radius.a*0.1 * dist * dist;
        vec3 dir = normalize(p - center);
        float ds = distance(p, center) - getplanetheight(dir);
        s += smoothstep(0.0, 0.1 * dist, ds) ;
        dist += 0.2;
    }
    return clamp(s, 0.0, 1.0);
}

vec4 tracePlanet(Ray ray){
    Sphere surface = Sphere(currentPlanet.position_radius.rgb, currentPlanet.position_radius.a);
    float atmoradius = currentPlanet.position_radius.a + currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b;
    Sphere atmosphere = Sphere(currentPlanet.position_radius.rgb,
        atmoradius);
    vec3 color = vec3(0.0);
    float coverage = 0.0;
    vec2 hit_Atmosphere = rsi2(ray, atmosphere);
    float hit_Surface = hits(hit_Atmosphere.x) || hits(hit_Atmosphere.y) ? raymarchTerrain(ray, currentPlanet.position_radius.xyz) : 0.0;//rsi2(ray, surface).x;

    float centerDistance = distance(ray.o, currentPlanet.position_radius.rgb);

        /* It has multiple scenarios possible to happen
        1. Ray origin is outside of atmosphere
            a: Ray hits nothing - return nothing
            b: Ray hits atmosphere but not the ground - trace atmosphere and return
            c: Ray hits both atmosphere and ground - trace planet then atmosphere, alpha mix and return

        2. Ray origin is in atmosphere but not in the ground
            a: Ray doesnt hit ground - trace atmosphere and return
            b: Ray hits ground - trace planet then atmosphere, alpha mix and return

        3. Ray origin is in the ground
            Ray will ALWAYS hit the ground so basically return black color with full alpha

        */

    int hitcount = 0;
    if(hits(hit_Surface)) hitcount++;
    if(hits(hit_Atmosphere.x) || hits(hit_Atmosphere.y)) hitcount++;
    //if(hitcount == 0) return vec4(0.0, 0.0, 0.0, -1.0);

    if(centerDistance >= currentPlanet.position_radius.a && centerDistance >= atmoradius){
        // scenario 1
        if(hitcount == 0) coverage = -1.0;//scenario a
        if(hitcount == 1){ // scenario b
            vec4 atm = tracePlanetAtmosphere(ray.d * hit_Atmosphere.x, ray.d * hit_Atmosphere.y,
                currentPlanet.position_radius.a  - maxheight, atmoradius);
            color = atm.rgb;
            coverage = atm.a;
        }
        if(hitcount == 2){ // scenario c
            vec4 atm = tracePlanetAtmosphere(ray.d * hit_Atmosphere.x, ray.d * hit_Surface,
                currentPlanet.position_radius.a  - maxheight, atmoradius);
            vec3 norm = getplanetnormal(normalize((ray.d * hit_Surface) - currentPlanet.position_radius.rgb));
            color = atm.rgb  + (1.0 - atm.a) * vec3(currentPlanet.preferredColor_zero.rgb) * max(0.0, dot(norm, normalize(SUNDIR - (ray.d * hit_Surface))));
            coverage = 1.0;
        }
    } else if(centerDistance >= currentPlanet.position_radius.a && centerDistance < atmoradius){
        if(hitcount == 1){ // scenario b
            vec4 atm = tracePlanetAtmosphere(ray.o, ray.d * hit_Atmosphere.y,
                currentPlanet.position_radius.a  - maxheight, atmoradius);
            color = atm.rgb;
            coverage = atm.a;
        }
        if(hitcount == 2){ // scenario c
            vec4 atm = tracePlanetAtmosphere(ray.o, ray.d * hit_Surface,
                currentPlanet.position_radius.a  - maxheight, atmoradius);
            vec3 norm = getplanetnormal(normalize((ray.d * hit_Surface) - currentPlanet.position_radius.rgb));
            color = atm.rgb  + (1.0 - atm.a) * vec3(currentPlanet.preferredColor_zero.rgb) * max(0.0, dot(norm, normalize(SUNDIR - (ray.d * hit_Surface))));
            coverage = 1.0;
        }
    } else if(centerDistance < currentPlanet.position_radius.a && centerDistance < atmoradius){
        coverage = 1.0; // scenario 3 return black full alpha
    }

    if(currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.r <= 0.001){
        vec4 rings = traceRings(ray, surface, normalize(SUNDIR - currentPlanet.position_radius.rgb));
        //color += (1.0 - coverage) * rings.xyz;
        //coverage += rings.a;

    }

/*
    if(!hits(hit_Surface) && !hits(hit_Atmosphere.x)) return vec4(0.0, 0.0, 0.0, -1.0);
    if(!hits(hit_Surface) && hits(hit_Atmosphere.x)){
        // Only ATMOSPHERE hit
        vec4 atm = tracePlanetAtmosphere(ray.d * hit_Atmosphere.x, ray.d * hit_Atmosphere.y,
            currentPlanet.position_radius.a,
            currentPlanet.position_radius.a
            + currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b);
        color = atm.rgb;
        coverage = atm.a;
    }
    if(hits(hit_Surface) && hits(hit_Atmosphere.x)) {
        vec4 atm = tracePlanetAtmosphere(ray.d * hit_Atmosphere.x, ray.d * hit_Surface,
            currentPlanet.position_radius.a, currentPlanet.position_radius.a + currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b);
        vec3 norm = getplanetnormal(normalize((ray.d * hit_Surface) - currentPlanet.position_radius.rgb));
        float plhei = getplanetheight(normalize((ray.d * hit_Surface) - currentPlanet.position_radius.rgb));
        float plheidelta = (plhei - currentPlanet.position_radius.a) / currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.r;
        vec3 newdir = plhei * normalize((ray.d * hit_Surface) - currentPlanet.position_radius.rgb);
        vec3 sundir = normalize(SUNDIR - ray.d * hit_Surface);
        vec2 hit_Atmosphere2 = rsi2(Ray(newdir, sundir), atmosphere);
        float shadow = smoothstep(-0.2 * plheidelta, 0.2, dot(sundir, newdir));
        color = atm.rgb * atm.a + (1.0 - atm.a) * shadow * vec3(currentPlanet.preferredColor_zero.rgb) * max(0.0, dot(norm, normalize(SUNDIR - (ray.d * hit_Surface))));
        coverage = 1.0;
    }
*/
    return vec4(color, min(1.0, coverage));
}
// END PLANET RENDERING

void main() {
    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
    Ray cameraRay = Ray(vec3(0.0), dir);
    vec4 oz = vec4(0.0);
    float lastdistance = 99999999.0;
    for(int i=0;i<planetsBuffer.count.x;i++){
        currentPlanet = planetsBuffer.planets[i];
        float dist = length(currentPlanet.position_radius.rgb);
        //if(dist * dist > 99999.0) continue;
        planetHostStar = starsBuffer.stars[currentPlanet.hoststarindex_zero_zero_zero.x];
        vec4 pl = tracePlanet(cameraRay);
        //if(pl.a < -0.9) continue;
        storeFragment(dist, max(pl.a, 0.0), pl.rgb);
        if(dist < lastdistance){ // current planet is closer than everything rendered yet
            oz.rgb = mix(oz.rgb, pl.rgb, pl.a);
            oz.a = pl.a;
            lastdistance = dist;
        } else {// current planet is not closer -> results is last color, as we dont have any info where it fits
            oz.rgb = mix(pl.rgb, oz.rgb,  oz.a);
            oz.a = 1.0;
        }
    }
    for(int i=0;i<moonsBuffer.count.x;i++){
        currentMoon = moonsBuffer.moons[i];
        moonHostPlanet = planetsBuffer.planets[currentMoon.hostplanetindex_zero_zero_zero.x];
        moonHostStar = starsBuffer.stars[moonHostPlanet.hoststarindex_zero_zero_zero.x];
        float dist = length(currentMoon.position_radius.rgb);
        //if(dist * dist > 99999.0) continue;
        vec4 pl = traceMoon(cameraRay);
        //if(pl.a < -0.9) continue;
        storeFragment(dist, max(pl.a, 0.0), pl.rgb);
        if(dist < lastdistance){ // current planet is closer than everything rendered yet
            oz.rgb = mix(oz.rgb, pl.rgb, pl.a);
            oz.a = pl.a;
            lastdistance = dist;
        } else {// current planet is not closer -> results is last color, as we dont have any info where it fits
            oz.rgb = mix(pl.rgb, oz.rgb,  oz.a);
            oz.a = 1.0;
        }
    }
    oz.rgb += (1.0 / 128.0) * vec3(rand2s(UV));
    outColor = resolveFragments();
}
