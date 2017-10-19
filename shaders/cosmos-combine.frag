#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texCelestial;

void main() {
    vec4 celestial = texture(texCelestial, UV);
    outColor = vec4(celestial.rgb, 1.0);
}
