#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texStars;
layout(set = 0, binding = 1) uniform sampler2D texPlanets;

void main() {
    vec4 stars = texture(texStars, UV);
    vec4 planets = texture(texPlanets, UV);
    vec3 color = mix(stars.rgb, planets.rgb, planets.a);
    outColor = vec4(color, 1.0);
}
