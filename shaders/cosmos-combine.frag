#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texStars;
layout(set = 0, binding = 1) uniform sampler2D texPlanets;

void main() {
    vec4 stars = texture(texStars, UV);
    vec4 planets = texture(texPlanets, UV);
    vec3 result = vec3(0.0);
    result += mix(stars.rgb, planets.rgb, step(0.0, planets.a - stars.a));
    if(planets.a == 0.0){
        result = stars.rgb;
    } else if(planets.a < stars.a){
        result = planets.rgb;
    } else if(stars.a < planets.a){
        result = planets.rgb + stars.rgb;
    }
    if(planets.a < 0.0) result += planets.rgb;
    outColor = vec4(result, 1.0);
}
