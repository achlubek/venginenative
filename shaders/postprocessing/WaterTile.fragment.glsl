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
#define cosinelinear(a) (cos(a * 3.1415 * 2.0) * 0.5 + 0.5)
#define snoisesinpow(a,b) pow(1.0 - abs(supernoise(a) - 0.5) * 2.0, b)
#define snoisesinpow2(a,b) pow(cosinelinear(supernoise(a)), b)

float heightwaterHI2(vec2 pos){
    float res = 0.0;
    pos *= 16.0;
    float w = 1.0;
    float wz = 1.0;
    float chop = 4.0;
    float tmod = 811.1 * WaterSpeed;

    for(int i=0;i<6;i++){
        vec2 t = vec2(tmod * Time*0.001);
        res += wz * snoisesinpow2(pos + t, chop);
        res += wz * snoisesinpow2(pos - t, chop);
        chop = mix(chop, 3.0, 0.3);
        w += wz * 2.0;
        wz *= 0.6;
        pos *= vec2(1.9, 1.7);
        tmod *= 0.3;
    }
    return res / w;
}
float heightwaterHI3(vec2 pos){
    float res = 0.0;
    pos *= 6.0;
    float w = 1.0;
    float wz = 1.0;
    float chop = 1.0;
    float tmod = 121.1 * WaterSpeed;

    vec2 t = vec2(tmod * Time*0.0001);
    float sn1 = supernoise3dX(vec3(pos * vec2(0.99, 0.5), Time * 0.4));
    float sn2 = supernoise3dX(vec3(pos * vec2(0.99, 0.5) + vec2(1.0, 0.0), Time * 0.4));
    float sn3 = supernoise3dX(vec3(pos * vec2(0.99, 0.5) + vec2(0.0, 1.0), Time * 0.4));
    vec2 dfx = abs(0.5 - vec2(sn2 - sn1, sn3 - sn1)) * 0.1 + 1.0;
    res += wz * pow(sn1, chop);
    wz *= 0.1;
    pos *=  dfx;

    chop = 1.0;
    res += wz * heightwaterHI2(pos);
    w += wz;
    return res / w;
}
float heightwaterHI(vec2 posx){
    float res = 0.0;
    posx *= 12.0;
    float w = 0.0;
    float wz = 1.0;
    float chop = 2.0;
    float tmod = 212.1 * WaterSpeed;

	vec3 pos = vec3(posx.x, posx.y, Time * 0.11 * WaterSpeed);
    vec3 t = vec3(tmod * Time*0.0001);
   // res += wz * snoisesinpow(pos + t, chop);
   // wz *= 0.4;
    //pos *= 2.0;
   // tmod *= 0.8;
    for(int i=0;i<6;i++){
        t = vec3(tmod * Time*0.001);
		float X = supernoise3dX(pos);
       // res += wz * pow(1.0 - abs(X - 0.5), chop);
       // res += wz * pow(sin(X * 3.1415) * 0.5 + 0.5, chop
	    res += wz * mix(pow(1.0 - abs(X - 0.5), chop), pow(sin(X * 3.1415) * 0.5 + 0.5, chop), 0.5);
     //   res += wz * snoisesinpow(pos - t, chop);
        chop = mix(chop, 4.0, 2.5);
        w += wz * 2.0;
        wz *= 0.4;
        pos.xy *= 2.0;
      //  pos.z *= 0.;
        //tmod *= 2.8;
    }
    return pow(res / w * 2.0, 3.0);
}

vec4 shade(){
    vec2 uv = UV;
    float c1 = heightwaterHI2(uv);
    float color = c1;
    return vec4(color, 0.0, 0.0, 0.0);
}
