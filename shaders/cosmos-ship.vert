#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out flat uint inInstanceId;
layout(location = 2) out vec3 outWorldPos;
layout(location = 3) out vec3 outNormal;

#include celestialSet.glsl

void main() {
    vec3 WorldPos = (shipBuffer.transformation
        * vec4(inPosition.xyz, 1.0)).rgb + shipBuffer.position.rgb;
    vec4 opo = (hiFreq.VPMatrix)
        * ((shipBuffer.transformation * vec4(inPosition.xyz, 1.0)) + vec4(shipBuffer.position.rgb, 0.0));
    vec3 Normal = inNormal;
    outNormal = normalize(Normal);
    outTexCoord = inTexCoord;
    outWorldPos = WorldPos;
    opo.y *= -1.0;
    gl_Position = opo;
}
