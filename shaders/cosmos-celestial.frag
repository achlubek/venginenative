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
} hiFreq;

struct GeneratedStarInfo {
    vec4 position_radius;
    vec4 color_age; //0->maybe 10? maybe 100?
    vec4 orbitPlane_rotationSpeed;
    vec4 spotsIntensity_zero_zero_zero; //0->1
};

struct GeneratedPlanetInfo {
    vec4 position_radius;
    vec4 terrainMaxLevel_fluidMaxLevel_starDistance_zero;
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
float FBM3(vec3 p, int octaves, float dx){
    float a = 0.0;
        float w = 0.5;
    for(int i=0;i<octaves;i++){
        a += noise3d(p) * w;
            w *= 0.5;
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

float rand2s(vec2 co){
    return fract(sin(dot(co.xy * Time,vec2(12.9898,78.233))) * 43758.5453);
}

Ray cameraRay;

// BEGIN PLANET RENDERING

GeneratedPlanetInfo currentPlanet;
GeneratedStarInfo planetHostStar;

vec3 extra_cheap_atmosphere(float raylen, float sunraylen, float absorbstrength, vec3 absorbcolor, float sunraydot){
    //sundir.y = max(sundir.y, -0.07);
    sunraydot = max(0.0, sunraydot);
    raylen *= absorbstrength * 0.001;
    sunraylen *= absorbstrength * 0.001;
    float special_trick = raylen;
    float special_trick2 = sunraylen;
    float raysundt = pow(abs(sunraydot), 2.0);
    float sundt = pow(max(0.0, sunraydot), 8.0);
    float mymie = sundt * special_trick * 0.2;
    vec3 suncolor = mix(vec3(1.0), max(vec3(0.0), vec3(1.0) - absorbcolor), special_trick2);
    vec3 bluesky= absorbcolor * suncolor;
    vec3 bluesky2 = max(vec3(0.0), bluesky - absorbcolor * 0.08 * (special_trick));
    bluesky2 *= special_trick * (0.24 + raysundt * 0.24);
    return bluesky2 + mymie * suncolor;
}
vec4 tracePlanetAtmosphere(vec3 start, vec3 end, float lengthstart, float lengthstop){
    vec4 result = vec4(0.0);
    float raylen = distance(start, end);
    vec3 sundir = normalize(planetHostStar.position_radius.rgb - end);
    Sphere atmosphere = Sphere(currentPlanet.position_radius.rgb,
        currentPlanet.position_radius.a + currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b);
    vec2 hit_Atmosphere = rsi2(Ray(end, sundir), atmosphere);
    vec3 planetdir1 = normalize(start - currentPlanet.position_radius.rgb);
    vec3 planetdir2 = normalize(end - currentPlanet.position_radius.rgb);
    float planetdirsundt1 = dot(sundir, planetdir1);
    float planetdirsundt2 = dot(sundir, planetdir2);
    float descent1 = smoothstep(-0.1, 0.2, planetdirsundt1);
    float descent2 = smoothstep(-0.1, 0.2, planetdirsundt2);
    float descent = (descent1 + descent2) * 0.5;
    vec3 atm = descent
        * extra_cheap_atmosphere(raylen, hit_Atmosphere.x, 15.0
            * currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.a,
            currentPlanet.atmosphereAbsorbColor_zero.rgb,
            max(0.0, dot(normalize(end - start), sundir)));
    return vec4(atm, 1.0);
}

float getplanetheight(vec3 dir){
    return currentPlanet.position_radius.a
        + (1.0 - 2.0 * abs(0.5 - FBM3(dir * 10.0, 6, 2.1)))
        * currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_zero.r;
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

#define hits(a) (a>0.0&&a<99999.0)
#line 227
vec4 tracePlanet(Ray ray){
    Sphere surface = Sphere(currentPlanet.position_radius.rgb, currentPlanet.position_radius.a);
    Sphere atmosphere = Sphere(currentPlanet.position_radius.rgb,
        currentPlanet.position_radius.a + currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b);
    vec3 color = vec3(0.0);
    float coverage = 0.0;
    //starColor = vec3(0.7)+vec3(hash(seed), hash(seed + 100.0), hash(seed + 200.0));

    float hit_Surface = rsi2(ray, surface).x;
    vec2 hit_Atmosphere = rsi2(ray, atmosphere);
    /*3 possible scenarios:
    nothing hit
    only atmosphere hit
    atmosphere and surface hit
    nothing is going to hit from the inside, you are not going into the star ok..
    */
    if(!hits(hit_Surface) && !hits(hit_Atmosphere.x)) return vec4(0.0, 0.0, 0.0, 9999999991.0);
    if(!hits(hit_Surface) && hits(hit_Atmosphere.x)){
        // Only ATMOSPHERE hit
        vec4 atm = tracePlanetAtmosphere(ray.d * hit_Atmosphere.x, ray.d * hit_Atmosphere.y,
            currentPlanet.position_radius.a,
            currentPlanet.position_radius.a
            + currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b);
        color = atm.rgb;
        //alphaMix = -1.0;
    //	coverage = 1.0;
    }
    if(hits(hit_Surface) && hits(hit_Atmosphere.x)) {
        vec4 atm = tracePlanetAtmosphere(ray.d * hit_Atmosphere.x, ray.d * hit_Surface,
            currentPlanet.position_radius.a, currentPlanet.position_radius.a + currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b);
        vec3 norm = getplanetnormal(normalize((ray.d * hit_Surface) - currentPlanet.position_radius.rgb));
        float plhei = getplanetheight(normalize((ray.d * hit_Surface) - currentPlanet.position_radius.rgb));
        float plheidelta = (plhei - currentPlanet.position_radius.a) / currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_zero.r;
        vec3 newdir = plhei * normalize((ray.d * hit_Surface) - currentPlanet.position_radius.rgb);
        vec3 sundir = normalize(planetHostStar.position_radius.rgb - ray.d * hit_Surface);
        vec2 hit_Atmosphere2 = rsi2(Ray(newdir, sundir), atmosphere);
        float shadow = smoothstep(-0.2 * plheidelta, 0.2, dot(sundir, newdir));
        color = atm.rgb + shadow * vec3(currentPlanet.preferredColor_zero.rgb) * max(0.0, dot(norm, normalize(planetHostStar.position_radius.rgb - (ray.d * hit_Surface))));
        //alphaMix = 1.0;
    }


    color += (1.0 / 128.0) * vec3(rand2s(UV));
    return vec4(color, min(hit_Surface, hit_Atmosphere.x));
}
// END PLANET RENDERING

void main() {
    Ray cameraRay = Ray(vec3(0.0), reconstructCameraSpaceDistance(UV, 1.0));
    vec4 oz = vec4(0.0);
    float lastdistance = 9999999990.0;
    for(int i=0;i<planetsBuffer.count.x;i++){
        currentPlanet = planetsBuffer.planets[i];
        planetHostStar = starsBuffer.stars[currentPlanet.hoststarindex_zero_zero_zero.x];
        vec4 pl = tracePlanet(cameraRay);
        oz.rgb += pl.rgb;
        /*if(pl.a < lastdistance){
            oz.rgb = pl.rgb;
            lastdistance = pl.a;
            oz.a = lastdistance;
        }*/
        //oz.a += pl.a;
    }
    //oz.a = min(1.0, oz.a);
    outColor = oz;
}
