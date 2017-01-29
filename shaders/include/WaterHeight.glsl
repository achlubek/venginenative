
uniform float WaterHeight;
uniform vec3 Wind;
uniform vec2 WaterScale;
float octavescale1 = 0.1;
float mipmap1 = 0.0;
float maxmip = textureQueryLevels(waterTileTex);

#include ProceduralValueNoise.glsl

vec2 heightwaterXO(vec2 uv, vec2 offset, float mipmap){

	vec2 zuv1 = (uv * WaterScale * octavescale1) + vec2(Time * 0.001 * WaterSpeed);
	vec2 zuv2 = zuv1 - vec2(Time * 0.0024 * WaterSpeed);
	vec2 zuv3 = zuv1 + vec2(Time * 0.0027 * WaterSpeed);
	vec2 zuv4 = zuv1 - vec2(Time * 0.0028 * WaterSpeed);
    vec2 a = textureLod(waterTileTex, zuv1, mipmap).rg;
    vec2 b = textureLod(waterTileTex, zuv2, mipmap).rg;
    vec2 c = textureLod(waterTileTex, zuv3, mipmap).rg;
    vec2 d = textureLod(waterTileTex, zuv4, mipmap).rg;
	return (a + b + c + d) * 0.55;
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
#define waterdepth 0.1 * WaterWavesScale
