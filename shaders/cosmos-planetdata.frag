#version 450
#extension GL_ARB_separate_shader_objects : enable

#include polar.glsl
#include proceduralValueNoise.glsl

layout(location = 0) in vec2 UV;
layout(location = 0) out float outHeight;
layout(location = 1) out vec4 outColorRoughness;
layout(location = 2) out vec4 outFlunctuationsClouds;

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

float noise4d2(vec4 a) {
    return (noise4d(a) + noise4d((a) + 100.5)) * 0.5;
}
float fbm(vec4 a){
    return noise4d2(a) * 0.5
        +noise4d2(a*2.0) * 0.25
        +noise4d2(a*4.0) * 0.125
        +noise4d2(a*8.0) * 0.065
        +noise4d2(a*16.0) * 0.032;
}
float fbm2(vec4 a){
    return fbm(a + (vec4(fbm(a + 100.0), fbm(a + 300.0), fbm(a + 600.0), 0.0) * 2.0 - 1.0));
}

vec3 getGroundColor(GeneratedPlanetInfo planet, float sunPower, float height){
    vec3 ice = vec3(0.9);
    vec3 mids = planet.preferredColor_zero.xyz;
    vec3 deserts = mix(vec3(1.0), planet.preferredColor_zero.xyz , 0.3);
    sunPower *= pow(1.0 - height, 4.0);
    float icemix = 1.0 - smoothstep(0.005, 0.00505, sunPower);
    float desertmix = smoothstep(0.4, 0.41, height);
    return mix(mix(deserts, mids, desertmix) , ice, icemix);
}
vec3 getWaterColor(GeneratedPlanetInfo planet){
    return vec3(0.2, 0.5, 0.8);
}

void main() {
    if(planetsBuffer.count.x > 0){
        vec3 dir = polarToXyz(UV);
        float clouds = smoothstep(0.45, 0.75, fbm2(vec4(dir * 3.0, 0.04 * hiFreq.Time)));
        GeneratedPlanetInfo planet = planetsBuffer.planets[0];
        float flunctuations = fbm2(vec4(dir * 3.0 * vec3(1.0, 5.0, 1.0), 0.04 * hiFreq.Time));
        float radius = planet.position_radius.a;
        float terrain_thickness = planet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.x * radius * 0.02;
        float water_level = planet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.y * 0.8;
        float height = fbm2(vec4(dir * 3.0, 0.0));
        float watermix = smoothstep(water_level, water_level+0.001, height);
        outHeight = max(water_level, height);
        outColorRoughness = mix(vec4(getGroundColor(planet, 1.0 - abs(dir.z), height), 1.0), vec4(getWaterColor(planet), 0.2), watermix);
        outFlunctuationsClouds = vec4(clouds, flunctuations, 0.0, 0.0);
    }
}
