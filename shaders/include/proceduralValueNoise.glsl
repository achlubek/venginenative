#pragma once


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
float FBM2(vec2 p, int octaves, float dx){
    float a = 0.0;
        float w = 0.5;
    for(int i=0;i<octaves;i++){
        a += noise2d(p) * w;
            w *= 0.5;
        p *= dx;
    }
    return a;
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
float FBM4(vec4 p, int octaves, float dx){
    float a = 0.0;
        float w = 0.5;
    for(int i=0;i<octaves;i++){
        a += noise4d(p) * w;
            w *= 0.5;
        p *= dx;
    }
    return a;
}

float configurablenoise(vec3 x, float h2, float h1) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*h1+ h2*p.z;
    return mix(mix(    mix( hash(n+0.0), hash(n+1.0),f.x),
        mix(hash(n+h1), hash(n+h1+1.0),f.x),f.y),
        mix(mix(hash(n+h2), hash(n+h2+1.0),f.x),
        mix(hash(n+h2+h1), hash(n+h2+h1+1.0),f.x),f.y),f.z);
}

#define noise3d(a) configurablenoise(a, 883.0, 971.0)

float supernoise3d(vec3 p){
    float a =  configurablenoise(p, 883.0, 971.0);
    float b =  configurablenoise(p + 0.5, 113.0, 157.0);
    return (a + b) * 0.5;
}
float supernoise3dX(vec3 p){
    float a =  configurablenoise(p, 883.0, 971.0);
    float b =  configurablenoise(p + 0.5, 113.0, 157.0);
    return (a * b);
}
