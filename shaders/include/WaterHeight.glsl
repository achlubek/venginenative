
uniform float WaterHeight;
uniform vec3 Wind;
uniform vec2 WaterScale;
float octavescale1 = 0.0002;
float mipmap1 = 0.0;
float maxmip = textureQueryLevels(waterTileTex);
float heightwaterX(vec2 uv, float mipmap){
    return textureLod(waterTileTex, uv * WaterScale * 0.0002 + vec2(Time, Time )* 0.0132 * WaterSpeed, mipmap).r;
}
float heightwaterD(vec2 uv, float mipmap){
    return heightwaterX(uv, mipmap * maxmip);
}
float heightwater(vec2 uv){
    return heightwaterX(uv, mipmap1 * 1.0);
}