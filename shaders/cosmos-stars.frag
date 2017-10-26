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

float rand2s(vec2 co){
    return fract(sin(dot(co.xy * Time,vec2(12.9898,78.233))) * 43758.5453);
}

float infinity = 1.0 / 0.0;
Ray cameraRay;
#define hits(a) (a > 0.0 && a < infinity)

// BEGIN STAR RENDERING

GeneratedStarInfo currentStar;

vec3 traceStarGlow(Ray ray){
    float dtraw = dot(normalize(currentStar.position_radius.rgb - ray.o), ray.d);
    float dotz = max(0.0, dtraw);

    float camdist = distance(CameraPosition, currentStar.position_radius.rgb);
    camdist *= 0.001;
    float dim = 1.0 - smoothstep(1000.0, 20000.0, camdist);
    camdist = min(camdist, 1000.0);
    float sqrlen = camdist * camdist;
    float specialtrick1 = 1.0 / (1.0 + 10.0 * sqrlen * (1.0 - dotz));

    return dim * specialtrick1 * currentStar.color_age.xyz * 3.0;
}
void main() {
    vec2 point = gl_PointCoord.xy * 2.0 - 1.0;
    currentStar = starsBuffer.stars[inInstanceId];
    outColor = vec4(currentStar.color_age.xyz * (1.0 - smoothstep(0.7, 1.0, length(point))), 1.0);

    Ray cameraRay = Ray(CameraPosition, reconstructCameraSpaceDistance(gl_FragCoord.xy / hiFreq.Resolution, 1.0));
    outColor = vec4(traceStarGlow(cameraRay), 1.0);

}
