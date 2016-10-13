#version 430 core

uniform vec3 SunDirection;
uniform float MieScattCoeff;

#include PostProcessEffectBase.glsl
#include Constants.glsl
#include PlanetDefinition.glsl
#include AtmScattering.glsl


vec4 shade(){
    vec3 val = getAtmosphereForDirectionReal(
        CameraPosition, 
        normalize(reconstructCameraSpaceDistance(UV, 1.0)), 
        normalize(dayData.sunDir));
    return vec4(val, 0);
}