#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform ubo {
    float Time;
} buff;
layout(set = 0, binding = 1) uniform sampler2D testTexture;

void main() {
    float levels = float(textureQueryLevels(testTexture));
    vec3 tex = textureLod(testTexture, UV, int(mod(buff.Time, levels))).rgb;
    tex.x += sin(buff.Time + UV.x) * 0.1;
    tex.y += sin(buff.Time*2.0 + UV.y) * 0.1;
    tex.z += sin(buff.Time*3.0 + UV.y + UV.x) * 0.1;
    outColor = vec4(tex, 1.0);
}
