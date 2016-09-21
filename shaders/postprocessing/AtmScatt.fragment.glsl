#version 430 core

#include PostProcessEffectBase.glsl

#define CLOUD_SAMPLES 256
#define CLOUDCOVERAGE_DENSITY 90
#include Atmosphere.glsl

vec4 shade(){
    vec3 val = getAtmosphereForDirectionRealX(CameraPosition, normalize(reconstructCameraSpaceDistance(UV, 1.0)), normalize(SunDirection));
    return vec4(val, 0);
}