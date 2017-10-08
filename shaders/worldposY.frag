#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inWorldPos;
layout(location = 0) out float outDistance;

#include sharedBuffers.glsl

void main() {
    outDistance = inWorldPos.y;
}
