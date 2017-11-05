#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 1) in flat uint inInstanceId;
layout(location = 2) in vec3 inWorldPos;
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
float rand2s(vec2 co){
    return fract(sin(dot(co.xy * Time,vec2(12.9898,78.233))) * 43758.5453);
}

float infinity = 1.0 / 0.0;
Ray cameraRay;
#define hits(a) (a > 0.0 && a < infinity)

// BEGIN STAR RENDERING

GeneratedStarInfo currentStar;

vec3 traceStarGlow(Ray ray){
    float dtraw = dot(normalize(currentStar.position_radius.rgb  - ray.o), ray.d);
    float dotz = max(0.0, dtraw);

    vec4 posradius = currentStar.position_radius;
    posradius.xyz -= CameraPosition;
    float dist = min(500000.0, length(posradius.xyz));
    float camdist = dist;

    vec3 reconpoint = ray.o + ray.d * camdist;
    float deltadst = distance(ray.o + normalize(posradius.xyz) * dist, reconpoint);
    float prcnt = max(0.0, 1.0 - deltadst / (currentStar.position_radius.a * 1.0));
    float prcnt2 = max(0.0, 1.0 - deltadst / (currentStar.position_radius.a * 4.0));
    prcnt *= prcnt;
    prcnt2 *= prcnt2 * prcnt2 * prcnt2 * prcnt2;
    camdist = distance(CameraPosition, currentStar.position_radius.xyz);
    camdist *= 0.001;
    //camdist = min(camdist, 66000.0);
    float cst2 = camdist * 0.001;
    float dim = clamp(1.0 /(1.0 + cst2 * cst2 * 0.06), 0.0001, 1.0);
    dim = pow(dim,2.0);
    camdist = min(camdist, 12000.0);
    camdist = min(camdist, 700.0);
    float sqrlen = camdist * camdist;
    float specialtrick1 = clamp(1.0 / max( 0.1, 10.0 * sqrlen * (1.0 - dotz) - 1.0), 0.0, 4.0);

return dim * (smoothstep(0.00, 0.3, prcnt) * 14.9 ) * currentStar.color_age.xyz;

    return dim * specialtrick1 * currentStar.color_age.xyz * 0.21;
}

void main() {
    currentStar = starsBuffer.stars[inInstanceId];
    vec4 posradius = currentStar.position_radius;
    posradius.xyz -= CameraPosition;
    float dist = min(300000.0, length(posradius.xyz));
    Ray cameraRay = Ray(CameraPosition, reconstructCameraSpaceDistance(gl_FragCoord.xy / hiFreq.Resolution, 1.0));
    Sphere surface = Sphere( normalize(posradius.xyz) * dist + CameraPosition, currentStar.position_radius.a);
    vec2 hit = rsi2(cameraRay, surface);
    float hitmult = hits(hit.x) ? 1.0 : 0.0;
    vec3 hitnorm = normalize((cameraRay.o + cameraRay.d * hit.x) - posradius.xyz);
    float camdist = distance(CameraPosition, currentStar.position_radius.rgb);
    camdist *= 0.001;
    camdist = min(camdist, 20000.0);
    float dim = 1.0 - smoothstep(1000.0, 30000.0, camdist);
    vec3 a = dim * currentStar.color_age.rgb * hitmult * 2.0;
    outColor = vec4(traceStarGlow(cameraRay), 1.0);

}
