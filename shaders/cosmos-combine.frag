#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texCelestial;
layout(set = 0, binding = 1) uniform sampler2D texStars;

void main() {
    vec4 celestial = texture(texCelestial, UV);
    vec4 stars = texture(texStars, UV);
    //stars.rgb /= max(0.0001, stars.a);
    outColor = vec4(mix(stars.rgb, celestial.rgb, celestial.a), 1.0);
}
