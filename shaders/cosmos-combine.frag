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
vec3 blur(sampler2D ss, float radius){
    vec4 res = vec4(0.0, 0.0, 0.0, 0.001);
    vec2 ratio = vec2(1.0, Resolution.x/Resolution.y);
    vec2 seed = UV;
    for(int i=0;i<50;i++){
        float x = rand2s(seed) * 3.1415 * 2.0;
        seed += 1.0;
        float y = rand2s(seed);
        seed += 1.0;
        vec2 newuv = clamp(UV + vec2(sin(x), cos(x)) * y * y* radius * ratio, 0.001, 0.999);
        res += vec4(texture(ss, newuv).rgb, 1.0 - y) * (1.0 - y);
    }
    return res.xyz / res.w;
}

float hash( float n ){
    return fract(sin(n)*758.5453);
}
float noise2d( in vec2 x ){
    vec2 p = floor(x);
    vec2 f = smoothstep(0.0, 1.0, fract(x));
    float n = p.x + p.y*57.0;
    return mix(
        mix(hash(n+0.0),hash(n+1.0),f.x),
        mix(hash(n+57.0),hash(n+58.0),f.x),
        f.y
       );
}
float noise3d( in vec3 x ){
    vec3 p = floor(x);
        vec3 f = smoothstep(0.0, 1.0, fract(x));
    float n = p.x + p.y*157.0 + 113.0*p.z;

    return mix(mix(	mix( hash(n+0.0), hash(n+1.0),f.x),
            mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
           mix(	mix( hash(n+113.0), hash(n+114.0),f.x),
            mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}
// YOU ARE WELCOME! 4d NOISE
float noise4d(vec4 x){
    vec4 p=floor(x);
    vec4 f=smoothstep(0.,1.,fract(x));
    float n=p.x+p.y*157.+p.z*113.+p.w*971.;
    return mix(mix(mix(mix(hash(n),hash(n+1.),f.x),mix(hash(n+157.),hash(n+158.),f.x),f.y),
    mix(mix(hash(n+113.),hash(n+114.),f.x),mix(hash(n+270.),hash(n+271.),f.x),f.y),f.z),
    mix(mix(mix(hash(n+971.),hash(n+972.),f.x),mix(hash(n+1128.),hash(n+1129.),f.x),f.y),
    mix(mix(hash(n+1084.),hash(n+1085.),f.x),mix(hash(n+1241.),hash(n+1242.),f.x),f.y),f.z),f.w);
}
float FBM3(vec3 p, int octaves, float dx, float ww){
    float a = 0.0;
        float w = 0.5;
        float sw = 0.0;
    for(int i=0;i<octaves;i++){
        a += noise3d(p) * w;
            w *= ww;
        p *= dx;
    }
    return a;
}

vec3 CameraPosition = hiFreq.inCameraPos.xyz;
vec3 FrustumConeLeftBottom = hiFreq.inFrustumConeLeftBottom.xyz;
vec3 FrustumConeBottomLeftToBottomRight = hiFreq.inFrustumConeBottomLeftToBottomRight.xyz;
vec3 FrustumConeBottomLeftToTopLeft = hiFreq.inFrustumConeBottomLeftToTopLeft.xyz;

#include camera.glsl
void main() {
    vec4 celestial = texture(texCelestial, UV);
    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
    dir *= 2.0;
    vec3 stars = texture(texStars, UV).rgb ;//texture(texStars, UV);
    vec4 ui = texture(uiTexture, UV);
    //stars.rgb /= max(0.0001, stars.a);
    vec3 a = mix(stars, celestial.rgb, celestial.a);
    a = mix(a, ui.rgb, ui.a);
    outColor = vec4(gammacorrect(clamp(a * 0.1, 0.0, 10000.0)), 1.0);
}
