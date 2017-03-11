
uniform float WaterHeight;
uniform vec3 Wind;
uniform vec2 WaterScale;
float octavescale1 = 0.03;
float mipmap1 = 0.0;
float maxmip = textureQueryLevels(waterTileTex);

#include ProceduralValueNoise.glsl

float wave(vec2 uv, vec2 emitter, float speed, float phase){
	float dst = distance(uv, emitter);
	return pow((0.5 + 0.5 * sin(dst * phase - Time * speed * WaterSpeed)), 3.0);
}
float genwaves(vec2 position){
	float w = wave(position, vec2(-7.3, 1.4), 1.0, 4.5) * 0.8;
	w += wave(position, vec2(6.3, -2.4), 1.0, 5.0) * 0.8;

	w += wave(position, vec2(1.3, -6.4), 1.0, 4.0) * 0.5;
	w += wave(position, vec2(-0.3, 7.4), 1.0, 4.0) * 0.5;


	w += wave(position, vec2(-7.3, 5.4), 2.0, 24.0) * 0.1;
	w += wave(position, vec2(7.3, -2.4), 2.0, 22.0) * 0.1;

	w += wave(position, vec2(3.3, -6.4), 4.0, 22.0) * 0.08;
	w += wave(position, vec2(-1.3, 4.4), 4.0, 20.0) * 0.08;


    return w * 0.65;
}
vec2 heightwaterXO(vec2 uv, vec2 offset, float mipmap){

    float inith = genwaves(uv * 0.01735 * WaterScale) * (1.0 - smoothstep(0.0, 7.0, mipmap));

    uv += vec2(Time * WaterSpeed, 0.0);
	vec2 zuv1 = (uv * WaterScale * octavescale1) + vec2(Time * 0.01 * WaterSpeed);
	zuv1 = zuv1 - vec2(Time * 0.0124 * WaterSpeed * 1.4);
    vec2 a = textureLod(waterTileTex, zuv1, mipmap).rg;
//	return (a + b + c + d) * 0.13 + pow(supernoise3d(vec3(uv.x* WaterScale.x, uv.y* WaterScale.y, Time * WaterSpeed * 2.0) * 0.094 + vec3(Time * WaterSpeed, 0.0, 0.0) * 0.1), 2.0) * 0.87;
    return (a * 0.11 + inith) * 0.5;
}
float heightwaterXOLO(vec2 uv, vec2 offset, float mipmap){

    float inith = genwaves(uv * 0.01735 * WaterScale) * (1.0 - smoothstep(0.0, 7.0, mipmap));
    return (0.11 + inith) * 0.5;
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
