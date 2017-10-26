#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texCelestial;
layout(set = 0, binding = 1) uniform sampler2D texStars;
layout(set = 0, binding = 2) uniform sampler2D uiTexture;

void main() {
    vec4 celestial = texture(texCelestial, UV);
    vec4 stars = texture(texStars, UV);
    vec4 ui = texture(uiTexture, UV);
    //stars.rgb /= max(0.0001, stars.a);
    vec3 a = mix(stars.rgb, celestial.rgb, celestial.a);
    a = mix(a, ui.rgb, ui.a);
    outColor = vec4(a, 1.0);
}
