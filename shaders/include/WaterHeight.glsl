
uniform float WaterHeight;
uniform vec3 Wind;
uniform vec2 WaterScale;
float octavescale1 = 0.03;
float mipmap1 = 0.0;
float maxmip = textureQueryLevels(waterTileTex);

#include ProceduralValueNoise.glsl

float wave(vec2 uv, vec2 emitter, float speed, float phase, float timeshift){
	float dst = distance(uv, emitter);
	return pow(EULER, sin(dst * phase - (Time + timeshift) * speed)) / EULER;
}
vec2 wavedrag(vec2 uv, vec2 emitter){
	return normalize(uv - emitter);
}

float getwaves(vec2 position, int numiters){
	position *= 0.1;/*
	float w = wave(position, vec2(70.3, 60.4), 1.0, 14.0) * 0.5;
	w += wave(position, vec2(60.3, -55.4), 1.0, 5.0) * 0.5;

	w += wave(position, vec2(-74.3, 50.4), 1.0, 4.0) * 0.5;
	w += wave(position, vec2(-60.3, -70.4), 1.0, 4.7) * 0.5;


	w += wave(position, vec2(-700.3, 500.4), 2.1, 8.0) * 0.1;
	w += wave(position, vec2(30.3, -200.4), 2.4, 8.8) * 0.1;

	w += wave(position, vec2(40.3, -50.4), 2.6, 9.0) * 0.1;
	w += wave(position, vec2(70.3, 10.4), 2.7, 9.6) * 0.1;

	w += wave(position, vec2(30.3, -23.4), 2.0, 12.0) * 0.08;
	w += wave(position, vec2(-20.3, -4.4), 2.230, 13.0) * 0.08;

	w += wave(position, vec2(-100.3, -760.4), 2.0, 14.0) * 0.08;
	w += wave(position, vec2(-100.3, 400.4), 2.230, 15.0) * 0.08;

	w += wave(position, vec2(300.3, -760.4), 2.0, 22.0) * 0.08;
	w += wave(position, vec2(-300.3, -400.4), 2.230, 23.0) * 0.08;

	w += wave(position, vec2(-100.3, -760.4), 2.0, 24.0) * 0.08;
	w += wave(position, vec2(-100.3, 400.4), 2.230, 22.0) * 0.08;
        return w * 0.14;*/
    float iter = 0.0;
    float phase = 6.0;
    float speed = 2.0 * WaterSpeed;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    for(int i=0;i<numiters;i++){
        vec2 p = vec2(sin(iter), cos(iter)) * 30.0;
        float res = wave(position, p, speed, phase, 0.0);
        float res2 = wave(position, p, speed, phase, 0.006);
        position -= wavedrag(position, p) * (res - res2) * weight * 1.7 ;
        w += res * weight;
        iter += 12.0;
        ws += weight;
        weight = mix(weight, 0.0, 0.2);
        phase *= 1.2;
        speed *= 1.02;
    }
    return w / ws;
}



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
#define snoisesinpow4(a,b) pow(supernoise3d(a), b)
#define snoisesinpow5(a,b) pow(1.0 - abs(0.5 - supernoise3d(vec3(a, Time * 0.3 * WaterSpeed))) * 2.0, b)
#define snoisesinpow6(a,b) pow(1.0 - abs(0.5 - supernoise3d(vec3(a, Time * 0.3 * WaterSpeed))) * 2.0, b)

float heightwaterHI2(vec2 pos){
    float resx = 0.0;
    pos *= 12.0;
    float w = 0.0;
    float wz = 1.0;
    float chop = 3.0;
    float tmod = 810.1;

    for(int i=0;i<6;i++){
        vec2 t = vec2(tmod * Time*0.0018);
        float x1 = snoisesinpow4(vec3(pos + t, Time ), chop);
        resx += wz * x1;
        w += wz * 1.0;
        wz *= 0.3;
        pos *= vec2(2.1, 1.9);
        //tmod *= 0.8;
    }
    w *= 0.55;
    return (pow(resx / w * 2.0, 1.0));
}
float getwavesHI(vec2 uv, float details){
	return (getwaves(uv, 30));// + details * 0.027 * heightwaterHI2(uv * 0.1  );
}


vec2 heightwaterXO(vec2 uv, vec2 offset, float mipmap){

    return vec2(getwavesHI(uv * 0.01735 * WaterScale, 1.0) * (1.0 - smoothstep(0.0, 7.0, mipmap)), 0.0);

}
float heightwaterXOLO(vec2 uv, vec2 offset, float mipmap){

    return getwaves(uv * 0.01735 * WaterScale, 16) * (1.0 - smoothstep(0.0, 7.0, mipmap));

}


vec2 heightwaterXOX(vec2 uv, vec2 offset, float mipmap){

	vec2 zuv1 = (uv * WaterScale * octavescale1);
    vec2 a = textureLod(waterTileTex, zuv1, mipmap).rg;
	return (a);
}
vec2 heightwaterX(vec2 uv, float mipmap){
    return heightwaterXO(uv, vec2(0.0), mipmap);
}
float heightwaterD(vec2 uv, float mipmap){
    return heightwaterX(uv, mipmap).r;
}
float heightwaterE(vec2 uv, vec2 e, float mipmap){
    return heightwaterXO(uv, e / textureSize(waterTileTex, int(floor(mipmap))) , mipmap).r;
}
float heightwater(vec2 uv){
    return heightwaterD(uv, mipmap1 * 1.0).r;
}
#define WaterLevel WaterHeight
#define waterdepth 3.1 * WaterWavesScale
