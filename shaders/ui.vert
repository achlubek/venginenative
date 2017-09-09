#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

#include uiBuffer.glsl

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;

void main() {
    vec2 p = inPosition.xy * 0.5 + 0.5;
    p *= uiScale;
    p += uiPosition;
    gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);
    outTexCoord = inTexCoord;
}
