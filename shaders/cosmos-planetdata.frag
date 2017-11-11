#version 450
#extension GL_ARB_separate_shader_objects : enable

#include polar.glsl
#include proceduralValueNoise.glsl

layout(location = 0) in vec2 UV;
layout(location = 0) out float outHeight;
layout(location = 1) out vec4 outColorRoughness;
layout(location = 2) out vec4 outFlunctuationsClouds;

#include celestialSet.glsl
#include camera.glsl

float noise4d2(vec4 a) {
    return (noise4d(a) + noise4d((a) + 100.5)) * 0.5;
}
float fbm(vec4 a){
    return noise4d(a) * 0.5
        +noise4d(a*2.0) * 0.25
        +noise4d(a*4.0) * 0.125
        +noise4d(a*8.0) * 0.065
        +noise4d(a*16.0) * 0.032;
}
float fbm3d(vec3 a){
    return noise3d(a) * 0.5
        +noise3d(a*2.0) * 0.25
        +noise3d(a*4.0) * 0.125
        +noise3d(a*8.0) * 0.065
        +noise3d(a*16.0) * 0.032;
}
float fbm2(vec4 a){
    return fbm(a + (vec4(fbm(a + 100.0), fbm(a + 300.0), fbm(a + 600.0), 0.0) * 2.0 - 1.0));
}

float fbm3d2(vec3 a){
    return fbm3d(a + vec3(fbm3d(a + 100.0), fbm3d(a + 300.0), fbm3d(a + 600.0)) * 2.0 - 1.0);
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
        GeneratedPlanetInfo planet = planetsBuffer.planets[0];
        float seed = planet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.a;
        float cc1 = hash(seed);
        seed += 1.0;
        float cc2 = hash(seed);
        seed += 1.0;
        float clouds =  sqrt(sqrt((1.0 - abs(dir.z)))) * fbm3d2(dir * 17.0);
        float flunctuations = step(0.01, planet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.r) + (1.0 - step(0.01, planet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.r)) * fbm3d(dir * 2.0 * vec3(1.0, 4.0 + 5.0 * hash(seed), 1.0));
        seed += 1.0;
        float radius = planet.position_radius.a;
        float terrain_thickness = planet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.x * radius * 0.1;
        float water_level = planet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.y * 0.15;
        float height = pow(1.2 * fbm3d2(dir * 9.0) * fbm3d(dir * 2.0), 1.8 + 1.3 * hash(seed));
        float watermix = 1.0 - smoothstep(water_level, water_level+0.001, height);
        outHeight = max(water_level, height);
        outColorRoughness = mix(vec4(getGroundColor(planet, 1.0 - abs(dir.z), height), 1.0), vec4(getWaterColor(planet), 0.2), watermix);
        outFlunctuationsClouds = vec4(clouds, flunctuations, 0.0, 0.0);
    }
}
