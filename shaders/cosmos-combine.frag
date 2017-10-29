#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texCelestial;
layout(set = 0, binding = 1) uniform sampler2D texStars;
layout(set = 0, binding = 2) uniform sampler2D uiTexture;

layout(set = 0, binding = 3) uniform UniformBufferObject1 {
    float Time;
    float Zero;
    vec2 Mouse;
    mat4 VPMatrix;
    vec4 inCameraPos;
    vec4 inFrustumConeLeftBottom;
    vec4 inFrustumConeBottomLeftToBottomRight;
    vec4 inFrustumConeBottomLeftToTopLeft;
    vec2 Resolution;
} hiFreq;

float rand2s(vec2 co){
    return fract(sin(dot(co.xy * hiFreq.Time,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 Resolution = hiFreq.Resolution;

vec3 blur(sampler2D ss, float radius){
    vec4 res = vec4(0.0);
    vec2 ratio = vec2(1.0, Resolution.x/Resolution.y);
    for(float x=0.0 + rand2s(UV) * 0.3217;x<3.1415 * 2.0;x+=0.3217){
        for(float y= + rand2s(UV) *  0.1;y<1.0;y+=0.1){
            vec2 newuv = clamp(UV + vec2(sin(x + y), cos(x + y)) * y * radius * ratio, 0.001, 0.999);;
            res += vec4(texture(ss, newuv).rgb, 1.0) * (1.0 - y);
        }
    }
    return res.xyz / res.w;
}

void main() {
    vec4 celestial = texture(texCelestial, UV);
    vec3 stars = textureLod(texStars, UV, 0).rgb;//blur(texStars, 0.015).rgb;//texture(texStars, UV);
    vec4 ui = texture(uiTexture, UV);
    //stars.rgb /= max(0.0001, stars.a);
    vec3 a = mix(stars, celestial.rgb, celestial.a);
    a = mix(a, ui.rgb, ui.a);
    outColor = vec4(a, 1.0);
}
