#pragma once

float hash( float n ){
    return fract(sin(n)*758.5453);
}

float noise2d( in vec2 x ){
    vec2 p = floor(x);
    vec2 f = smoothstep(0.0, 1.0, fract(x));
    float n = p.x + p.y*57.0;
    return mix(mix(hash(n+0.0),hash(n+1.0),f.x),mix(hash(n+57.0),hash(n+58.0),f.x),f.y);
}

float configurablenoise(vec3 x, float h2, float h1) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*h1+ h2*p.z;
    return mix(mix(	mix( hash(n+0.0), hash(n+1.0),f.x),
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
