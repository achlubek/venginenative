#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 1) in flat uint inInstanceId;
layout(location = 2) in vec3 inWorldPos;
layout(location = 3) in vec3 inNormal;
layout(set = 0, binding = 8) uniform sampler2D texShipColor;
layout(location = 0) out vec4 outColor;

#include celestialSet.glsl
#include camera.glsl
#include sphereRaytracing.glsl
#include polar.glsl
#include transparencyLinkedList.glsl
#include rotmat3d.glsl

float rand2s(vec2 co){
    return fract(sin(dot(co.xy * Time,vec2(12.9898,78.233))) * 43758.5453);
}

Ray cameraRay;
void main() {
    outColor = vec4(texture(texShipColor, vec2(UV.x, 1.0 - UV.y)).rgb, 1.0);

}
