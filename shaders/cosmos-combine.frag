#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texCelestial;
layout(set = 0, binding = 1) uniform sampler2D texStars;
layout(set = 0, binding = 2) uniform sampler2D uiTexture;
layout(set = 0, binding = 4) uniform sampler2D texShip;

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
float rand2s2(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 Resolution = hiFreq.Resolution;

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x) {
   return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}
vec3 tonemapUncharted2(vec3 color) {
    float ExposureBias = 2.0;
    vec3 curr = Uncharted2Tonemap(ExposureBias * color);

    vec3 whiteScale = 1.0 / Uncharted2Tonemap(vec3(W));
    return curr * whiteScale;
}

vec3 gammacorrect(vec3 c){
    return pow(c, vec3(1.0 / 2.4));
}
vec4 blur(sampler2D ss, float radius){
    vec4 res = vec4(0.0, 0.0, 0.0, 0.0);
    float wsum = 0.001;
    vec2 ratio = vec2(1.0, Resolution.x/Resolution.y);
    vec2 seed = UV;
    for(int i=0;i<50;i++){
        float x = rand2s(seed) * 3.1415 * 2.0;
        seed += 1.0;
        float y = rand2s(seed);
        seed += 1.0;
        vec2 newuv = clamp(UV + vec2(sin(x), cos(x)) * y * radius * ratio, 0.001, 0.999);
        res += texture(ss, newuv).rgba;
        wsum += 1.0;
    }
    return res.xyzw / wsum;
}

vec4 boxynonoiseblur(sampler2D ss, int axissamples){
    vec4 res = vec4(0.0, 0.0, 0.0, 0.0);
    float wsum = 0.001;
    vec2 pixel = 1.0 / Resolution;
    for(int i=-axissamples;i<axissamples;i++){
        for(int g=-axissamples;g<axissamples;g++){
            vec2 newuv = clamp(UV + vec2(i*pixel.x, g*pixel.y), 0.001, 0.999);
            res += texture(ss, newuv).rgba;
            wsum += 1.0;
        }
    }
    return res.xyzw / wsum;
}

vec3 CameraPosition = hiFreq.inCameraPos.xyz;
vec3 FrustumConeLeftBottom = hiFreq.inFrustumConeLeftBottom.xyz;
vec3 FrustumConeBottomLeftToBottomRight = hiFreq.inFrustumConeBottomLeftToBottomRight.xyz;
vec3 FrustumConeBottomLeftToTopLeft = hiFreq.inFrustumConeBottomLeftToTopLeft.xyz;

#include camera.glsl
void main() {
    vec4 celestial = texture(texCelestial, UV);
    vec4 celestialblur = boxynonoiseblur(texCelestial, 2);
    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
    dir *= 2.0;
    vec3 stars = texture(texStars, UV).rgb ;//texture(texStars, UV);
    vec4 ui = texture(uiTexture, UV);
    //stars.rgb /= max(0.0001, stars.a);
    vec3 a = mix(stars, celestial.rgb + celestialblur.rgb * (1.0 - celestial.a), celestialblur.a);
    vec4 shipdata = texture(texShip, UV).rgba;
    vec4 shipdatablur = boxynonoiseblur(texShip, 2);
    a = mix(a, shipdata.rgb + shipdatablur.rgb * (1.0 - shipdata.a), shipdatablur.a);
    a = mix(a, ui.rgb, ui.a);
    outColor = vec4(gammacorrect(clamp(a * 0.1, 0.0, 10000.0)), 1.0);
}
