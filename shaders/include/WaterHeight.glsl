
uniform float WaterHeight;
uniform vec3 Wind;
uniform vec2 WaterScale;
float octavescale1 = 0.1;
float mipmap1 = 0.0;
float maxmip = textureQueryLevels(waterTileTex);

#include ProceduralValueNoise.glsl

vec2 heightwaterXO(vec2 uv, vec2 offset, float mipmap){
    uv += vec2(Time * WaterSpeed, 0.0);
	vec2 zuv1 = (uv * WaterScale * octavescale1) + vec2(Time * 0.01 * WaterSpeed);
	zuv1 = zuv1 - vec2(Time * 0.0124 * WaterSpeed * 1.4);
	vec2 zuv2 = zuv1 + vec2(Time * 0.024 * WaterSpeed * 1.4);
	vec2 zuv3 = zuv1 - vec2(Time * 0.027 * WaterSpeed * 1.4);
	vec2 zuv4 = zuv1 + vec2(Time * 0.028 * WaterSpeed * 1.4);
    vec2 a = textureLod(waterTileTex, zuv1, mipmap).rg;
    vec2 b = textureLod(waterTileTex, zuv2 * 1.04, mipmap).rg;
    vec2 c = textureLod(waterTileTex, zuv3 * 1.01, mipmap).rg;
    vec2 d = textureLod(waterTileTex, zuv4 * 0.98, mipmap).rg;
	return (a + b + c + d) * 0.13 + pow(supernoise3d(vec3(uv.x* WaterScale.x, uv.y* WaterScale.y, Time * WaterSpeed * 2.0) * 0.094 + vec3(Time * WaterSpeed, 0.0, 0.0) * 0.1), 2.0) * 0.87;
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
#define waterdepth 1.1 * WaterWavesScale
