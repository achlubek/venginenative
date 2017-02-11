#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 16) uniform sampler2D inputTex;
layout(binding = 23) uniform sampler2D bb;

uniform float Time;
uniform float WaterSpeed;


#include ProceduralValueNoise.glsl
/*
vec4 textureSurround(vec2 q, vec3 e, bool odd){

	float p10 = 0.0;
	float p01 = 0.0;
	float p21 = 0.0;
	float p12 = 0.0;
	if(odd){
		p10 = texture2D(bb, q-e.zy).y;
		p01 = texture2D(bb, q-e.xz).y;
		p21 = texture2D(bb, q+e.xz).y;
		p12 = texture2D(bb, q+e.zy).y;

	} else {

		p10 = texture2D(bb, q-e.zy).x;
		p01 = texture2D(bb, q-e.xz).x;
		p21 = texture2D(bb, q+e.xz).x;
		p12 = texture2D(bb, q+e.zy).x;
	}
	return vec4(p10, p01, p21, p12);
}
#include noise3D.glsl
#define ssnoise(a) (0.5 + 0.5 * snoise(a))
vec4 shade(){
	vec2 q = UV;
	vec3 e = vec3(vec2(1.0)/Resolution.xy,0.) * clamp(WaterSpeed, 0.01, 10.0);
	vec4 c = texture2D(bb, q);
	//q.x -= e.x;
	float p11 = c.a > 0.5 ? c.x : c.y;

	vec4 pp = textureSurround(q, e, c.a > 0.5);

	float d =  0.0;
	//p11 *= 1.001;
	d += -p11 + (pp.x + pp.y + pp.z + pp.w)*mix(0.4999, 0.50000, smoothstep(0.0, 0.4, WaterSpeed));
	//d += smoothstep(0.997,0.997,1.0 - length(mouse.xy - q.xy));
	float rain = 0.0;
	rain += smoothstep(0.99, 0.99, ssnoise(vec3(q.x * 40.0, q.y * 40.0, Time * 30.0)));
	rain += smoothstep(0.99, 0.99, ssnoise(vec3(q.x * -40.0, q.y * 40.0, Time * 30.0)));
	rain += smoothstep(0.99, 0.99, ssnoise(vec3(q.x * 40.0, q.y * -40.0, Time * 30.0)));
	rain += smoothstep(0.99, 0.99, ssnoise(vec3(q.x * -40.0, q.y * -40.0, Time * 30.0)));
	rain += smoothstep(0.99, 0.99, ssnoise(vec3(q.x * 40.0, q.y * 40.0, Time * 23.0)));
	rain += smoothstep(0.99, 0.99, ssnoise(vec3(q.x * 40.0, q.y * 40.0, Time * 14.0)));
	d += rain * 0.02 * WaterSpeed ;
	//d *= 1.0 - smoothstep(0.98, 1.0, d);
	d = clamp(d, 0.01, 7.0);
	if(c.a > 0.5){
		return vec4( d, c.y, 0.0, 1.0 - c.a );
	} else {
		return vec4( c.x, d, 0.0, 1.0 - c.a );
	}
}
*/


float hashx( float n ){
    return fract(sin(n)*758.5453);
}

float noise2X( vec2 x2 , float a, float b, float off){
	vec3 x = vec3(x2.x, x2.y,Time * 0.1 + off);
	vec3 p = floor(x);
	vec3 f = fract(x);
	f       = f*f*(3.0-2.0*f);

	float h2 = a;
	 float h1 = b;
	#define h3 (h2 + h1)

	 float n = p.x + p.y*h1+ h2*p.z;
	return mix(mix(	mix( hashx(n+0.0), hashx(n+1.0),f.x),
			mix( hashx(n+h1), hashx(n+h1+1.0),f.x),f.y),
		   mix(	mix( hashx(n+h2), hashx(n+h2+1.0),f.x),
			mix( hashx(n+h3), hashx(n+h3+1.0),f.x),f.y),f.z);
}

float supernoise(vec2 x){
    float a = noise2X(x, 55.0, 92.0, 0.0);
    float b = noise2X(x + 0.5, 133.0, 326.0, 0.5);
    return (a * b);
}
#define cosinelinear(a) ((cos(a * 3.1415) * -sin(a * 3.1415) * 0.5 + 0.5))
#define snoisesinpow(a,b) pow(1.0 - abs(supernoise3d(vec3(a, Time)) - 0.5) * 2.0, b)
#define XX(a,b) pow(1.0 - abs((a) - 0.5) * 2.0, b)
#define snoisesinpow2(a,b) pow(cosinelinear(supernoise(a)), b)
#define snoisesinpow3(a,b) pow(1.0 - abs(supernoise(a ) - 0.5) * 2.0, b)
#define snoisesinpow4X(a,b) pow(1.0 - 2.0 * abs(supernoise(a) - 0.5), b)
#define snoisesinpow4(a,b) pow(cosinelinear(1.0 - 2.0 * abs(supernoise(a) - 0.5)), b)
#define snoisesinpow5(a,b) pow(1.0 - abs(0.5 - supernoise3d(vec3(a, Time * 0.3 * WaterSpeed))) * 2.0, b)
#define snoisesinpow6(a,b) pow(1.0 - abs(0.5 - supernoise3d(vec3(a, Time * 0.3 * WaterSpeed))) * 2.0, b)

float heightwaterHI2(vec2 pos){
    float last = textureLod(bb, UV, 0.0).r;
    float res = 0.0;
    pos *= 6.0;
    float w = 0.0;
    float wz = 1.0;
    float chop = 6.0;
    float tmod = 5210.1 * WaterSpeed;

    for(int i=0;i<6;i++){
        vec2 t = vec2(tmod * Time*0.00018);
        float x1 = snoisesinpow4X(pos + t, chop);
        float x2 = snoisesinpow4(pos + t, chop);
        res += wz * mix(x1 * x2, x2, supernoise(pos + t) * 0.5 + 0.5) * 2.5;
        w += wz * 1.0;
        x1 = snoisesinpow4X(pos - t, chop);
        x2 = snoisesinpow4(pos - t, chop);
        res += wz * mix(x1 * x2, x2, supernoise(pos - t) * 0.5 + 0.5) * 2.5;
        w += wz * 1.0;
        chop = mix(chop, 5.0, 0.3);
        wz *= 0.4;
        pos *= vec2(2.1, 1.9);
        tmod *= 0.8;
    }
    //pos *= 0.99;
    //pos *= vec2(1.0, 1.4);
    /*chop = 2.0;
    wz *= 0.8;
    for(int i=0;i<5;i++){
        vec2 t = vec2(tmod );
        res += wz * snoisesinpow5(pos, chop) * 0.5;
        //res += wz * snoisesinpow6(pos - t, chop);
        chop = mix(chop, 3.0, 0.3);
        w += wz;
        wz *= 0.5;
        pos *= vec2(2.1, 1.9);
        tmod *= 0.8;
    }*/
    w *= 0.55;
    return mix(pow(res / w * 2.0, 2.0), last, 0.95);
}
vec4 shade(){
    vec2 uv = UV;
    float c1 = heightwaterHI2(uv);
    float color = c1;
    return vec4(color, 0.0, 0.0, 0.0);
}
