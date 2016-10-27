#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 16) uniform sampler2D inputTex;
layout(binding = 23) uniform sampler2D waterTileTex;

uniform float Time;
uniform float WaterSpeed;


float hashX( float n ){
    return fract(sin(n)*758.5453);
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
#define cosinelinear(a) (cos(a * 3.1415 * 2.0))
#define snoisesinpow(a,b) pow(sin(noise2X(a) * 3.1415 * 2.0) , b) 
float heightwaterHI(vec2 pos){
    float res = 0.0;
    pos *= 6.0;
    float w = 0.0;
    float wz = 1.0;
    float chop = 6.0;
    float tmod = 22.1 * WaterSpeed;
    float rotmod = Time * 0.005 * WaterSpeed;
    float rotmod2 = Time * 1.2134 * 0.005 * WaterSpeed;
    mat2 rmat = mat2(cos(rotmod), -sin(rotmod), sin(rotmod), cos(rotmod));
    mat2 rmat2 = mat2(cos(rotmod2), -sin(rotmod2), sin(rotmod2), cos(rotmod2));
    vec2 v = rmat * vec2(0.5, 0);
    vec2 v2 = rmat2 * vec2(0.5, 0);
    for(int i=0;i<4;i++){
        vec2 t = v * (tmod * Time*0.001);
        vec2 t2 = v2 * (tmod * Time*0.001);
        res += wz * snoisesinpow(pos + t, chop);
        res += wz * snoisesinpow(pos - t2, chop);
        chop = mix(chop, 1.0, 0.1);
        w += wz * 2.0;
        wz *= 0.4;
        pos *= 2.2;
        //tmod *= 0.8;
    }
    return res / w;
}

float foam(float val){
    float oldval = textureLod(waterTileTex, UV, 0.1).r;
    float oldval2 = textureLod(waterTileTex, UV, textureQueryLevels(waterTileTex)).r;
    float oldfoam = textureLod(waterTileTex, UV, 0.0).g * 0.999987;
    float diff = pow(max(0.0, (oldval - val) / (oldval - oldfoam)) * 111.0, 16.0);
    return min(1.0, diff + oldfoam);
}

vec4 shade(){
    vec2 uv = UV;
    float c1 = heightwaterHI(uv);
    float color = c1;
    return vec4(color, foam(color) * color, 0.0, 0.0);
}