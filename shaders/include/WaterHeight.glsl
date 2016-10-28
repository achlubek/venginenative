
uniform float WaterHeight;
uniform vec3 Wind;
uniform vec2 WaterScale;
float octavescale1 = 0.0001;
float mipmap1 = 0.0;
float maxmip = textureQueryLevels(waterTileTex);
vec2 heightwaterX(vec2 uv, float mipmap){
    return textureLod(waterTileTex, uv * WaterScale * 0.0001 + vec2(Time, Time )* 0.0012 * length(WaterScale) * WaterSpeed, mipmap).rg;
}
float heightwaterD(vec2 uv, float mipmap){
    return heightwaterX(uv, mipmap ).r;
}
float heightwater(vec2 uv){
    return heightwaterD(uv, mipmap1 * 1.0).r;
}