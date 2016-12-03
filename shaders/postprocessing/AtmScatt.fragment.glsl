#version 430 core

uniform vec3 SunDirection;
uniform float MieScattCoeff;

#include PostProcessEffectBase.glsl
#include Constants.glsl
#include PlanetDefinition.glsl
#include ProceduralValueNoise.glsl
#include AtmScattering.glsl


vec4 shade(){
    vec3 val = getAtmosphereForDirectionReal(
        vec3(0.0), 
        normalize(reconstructCameraSpaceDistance(UV, 1.0)), 
        normalize(dayData.sunDir));
    return vec4(val, 0);
}