#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 16) uniform sampler2D inputTex;

uniform float Time;
uniform float WaterSpeed;


float hashX( float n ){
    return fract(sin(n)*758.5453);
}

float noiseX( in vec3 x ){
    vec3 p = floor(x);
    vec3 f = fract(x); 
    float n = p.x + p.y*57.0 + p.z*800.0;
    float res = mix(mix(mix( hashX(n+  0.0), hashX(n+  1.0),f.x), mix( hashX(n+ 57.0), hashX(n+ 58.0),f.x),f.y),
    mix(mix( hashX(n+800.0), hashX(n+801.0),f.x), mix( hashX(n+857.0), hashX(n+858.0),f.x),f.y),f.z);
    return res;
}
float noise2X( in vec2 x ){
    vec2 p = floor(x);
    vec2 f = fract(x); 
    float n = p.x + p.y*57.0;
    float res = 
        mix (
            mix (
                hashX (
                    n + 0.0
                ), hashX (
                    n + 1.0
                ), f.x
            ), mix (
                hashX (
                    n + 57.0
                ), hashX (
                    n + 58.0
                ), f.x
            ), f.y
        );
    return res;
}

#include noise3D.glsl
#include noise2D.glsl

#define cosinelinear(a) smoothstep(0.0, 1.0, a)
#define snoisesinpow(a,b) pow(1.0 - cosinelinear(abs(snoise2d(a) )), b)
float heightwaterHI(vec2 pos){
    float res = 0.0;
    float w = 0.0;
    float wz = 1.0;
    float chop = 6.0;
    float tmod = 22.1 * WaterSpeed;
    for(int i=0;i<6;i++){
        vec2 t = vec2(noise2X(pos)*2.0 +  tmod * Time*0.001);
        res += wz * snoisesinpow(pos + t.yx, chop);
        res += wz * snoisesinpow(pos - t.yx, chop);
        chop = mix(chop, 0.7, 0.4);
        w += wz * 2.0;
        wz *= 0.4;
        pos *= 2.8;
        tmod *= 1.8;
    }
    return res / w;
}
vec4 shade(){
    vec2 uv = UV;
    float c1 = heightwaterHI(uv);
    float color = c1;
    return vec4(color);
}