#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform ubo {
    float Time;
} buff;
//layout(set = 0, binding = 1) uniform sampler2D testTexture;

void main() {
    vec3 tex = vec3(0.0);//texture(testTexture, UV).rgb;
    tex.x += sin(buff.Time + UV.x);
    tex.y += sin(buff.Time*2.0 + UV.y);
    tex.z += sin(buff.Time*3.0 + UV.y + UV.x);
    outColor = vec4(tex, 1.0);
}
