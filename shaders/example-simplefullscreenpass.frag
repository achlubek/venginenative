#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform ubo {
    float Time;
} buff;

void main() {
    outColor = vec4(sin(buff.Time + UV.x), sin(buff.Time*2.0 + UV.y), sin(buff.Time*3.0 + UV.y + UV.x), 1.0);
}
