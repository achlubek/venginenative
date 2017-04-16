float octavescale1 = 0.03;
float mipmap1 = 0.0;

#include ProceduralValueNoise.glsl

float wave(vec2 uv, vec2 emitter, float speed, float phase, float timeshift){
	float dst = distance(uv, emitter);
	return pow(EULER, sin(dst * phase - (Time + timeshift) * speed));
}
vec2 wavedrag(vec2 uv, vec2 emitter){
	return normalize(uv - emitter);
}

float getwaves(vec2 position, int numiters){
    float iter = 0.0;
    float phase = 6.0;
    float speed = 2.0 * WaterSpeed;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    float iwaterspeed = 1.0 / WaterSpeed;
    for(int i=0;i<numiters;i++){
        vec2 p = vec2(sin(iter), cos(iter)) * 30.0;
        float res = wave(position, p, speed, phase, 0.0) * IEULER;
        float res2 = wave(position, p, speed, phase, 0.006) * IEULER;
        position -= wavedrag(position, p) * (res - res2) * weight * 4.0 * iwaterspeed;
        w += res * weight;
        iter += 12.0;
        ws += weight;
        weight = mix(weight, 0.0, 0.2);
        phase *= 1.2;
        speed = pow(speed, 1.04);
    }
    return w / ws;
}

vec2 getFlow(vec2 position, int numiters){
    float iter = 0.0;
    float phase = 6.0;
    float speed = 2.0 * WaterSpeed;
    float weight = 1.0;
    vec2 w = vec2(0.0);
    float ws = 0.0;
    float iwaterspeed = 1.0 / WaterSpeed;
    for(int i=0;i<numiters;i++){
        vec2 p = vec2(sin(iter), cos(iter)) * 30.0;
        float res = wave(position, p, speed, phase, 0.0) * IEULER;
        float res2 = wave(position, p, speed, phase, 0.006) * IEULER;
        w += -wavedrag(position, p) * (res - res2) * weight * 4.0 * iwaterspeed;
        position -= wavedrag(position, p) * (res - res2) * weight * 4.0 * iwaterspeed;
        //w += res * weight;
        iter += 12.0;
        ws += weight;
        weight = mix(weight, 0.0, 0.2);
        phase *= 1.2;
        speed = pow(speed, 1.04);
    }
    return w / ws;
}
float getwavesHI(vec2 uv, float details){
	return (getwaves(uv, 47));// + details * 0.027 * heightwaterHI2(uv * 0.1  );
}


vec2 heightwaterXO(vec2 uv, vec2 offset, float mipmap){

    return vec2(getwavesHI(uv * 0.01735 * WaterScale, 1.0) * (1.0 - smoothstep(0.0, 7.0, mipmap)), 0.0);

}
float heightwaterXOLO(vec2 uv, vec2 offset, float mipmap){

    return getwaves(uv * 0.01735 * WaterScale, 7) * (1.0 - smoothstep(0.0, 7.0, mipmap));

}
vec2 heightflow(vec2 uv){

    return getFlow(uv * 0.01735 * WaterScale, 17);// * (1.0 - smoothstep(0.0, 7.0, mipmap));

}
vec2 heightwaterX(vec2 uv, float mipmap){
    return heightwaterXO(uv, vec2(0.0), mipmap);
}
float heightwaterD(vec2 uv, float mipmap){
    return heightwaterX(uv, mipmap).r;
}
float heightwaterE(vec2 uv, vec2 e, float mipmap){
    return heightwaterXO(uv, e, mipmap).r;
}
float heightwater(vec2 uv){
    return heightwaterD(uv, mipmap1 * 1.0).r;
}
#define WaterLevel WaterHeight
#define waterdepth 0.2 * WaterWavesScale
