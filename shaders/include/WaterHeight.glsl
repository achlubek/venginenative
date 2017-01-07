
uniform float WaterHeight;
uniform vec3 Wind;
uniform vec2 WaterScale;
float octavescale1 = 0.1;
float mipmap1 = 0.0;
float maxmip = textureQueryLevels(waterTileTex);


#define cosinelinear(a) (cos(a * 3.1415 * 2.0) * 0.5 + 0.5)
#define snoisesinpow(a,b) pow(abs(supernoise3dX(a) ), b)
#define snoisesinpow2(a,b) pow(cosinelinear(supernoise(a)), b)


float heightwaterHI(vec2 posx){
    float res = 0.0;
    posx *= 2.0;
    float w = 0.0;
    float wz = 1.0;
    float chop = 1.0;
    float tmod = 212.1 * WaterSpeed;

	vec3 pos = vec3(posx.x, posx.y, Time * 1.01);
    vec3 t = vec3(tmod * Time*0.0001);
   // res += wz * snoisesinpow(pos + t, chop);
   // wz *= 0.4;
    //pos *= 2.0;
   // tmod *= 0.8;
    for(int i=0;i<4;i++){
        t = vec3(tmod * Time*0.001);
        res += wz * snoisesinpow(pos , chop);
     //   res += wz * snoisesinpow(pos - t, chop);
        //chop = mix(chop, 3.0, 0.5);
        w += wz * 2.0;
        wz *= 0.3;
        pos *= 2.0;
        tmod *= 2.8;
    }
    return res / w;
}


vec2 heightwaterXO(vec2 uv, vec2 offset, float mipmap){
    #define xns(a) pow(supernoise3dX(a) * 0.5 + 0.5, 3.0)
    float sn1 = xns(vec3(uv * WaterScale * octavescale1 * vec2(0.99, 0.5) * 3.0 + vec2(0.0 + Time * 0.2 * WaterSpeed, 0.0), Time * 0.24 * WaterSpeed)) * WaterHeight;
    //float sn2 = xns(vec3(uv * WaterScale * octavescale1 * vec2(0.99, 0.5) * 3.0 + vec2(0.01 + Time * 0.2 * WaterSpeed, 0.0), Time * 0.24 * WaterSpeed)) * WaterHeight;
    //float sn3 = xns(vec3(uv * WaterScale * octavescale1 * vec2(0.99, 0.5) * 3.0 + vec2(0.0 + Time * 0.2 * WaterSpeed, 0.01), Time * 0.24 * WaterSpeed)) * WaterHeight;
    //vec2 dfx =  (vec2(sn2 - sn1, sn3 - sn1));
    //if(UV.x < 0.5)uv += 50.0 * mix(dfx, vec2(1.0), smoothstep(0.0, 0.3, mipmap / maxmip));
    //sn1 = mix(sn1, 0.5, smoothstep(0.0, 0.7, mipmap / maxmip));
	return textureLod(waterTileTex, uv * 0.5 * WaterScale * octavescale1 + offset, mipmap).rg * 0.017 + vec2(sn1, sn1) * 1.0;
    /*
	vec2 zuv1 = (uv * WaterScale * octavescale1) + vec2(Time * 0.001 * WaterSpeed);
	vec2 zuv2 = zuv1 - vec2(Time * 0.0014 * WaterSpeed);
	vec2 zuv3 = zuv1 + vec2(Time * 0.0017 * WaterSpeed);
	vec2 zuv4 = zuv1 - vec2(Time * 0.0018 * WaterSpeed);
    vec2 a = textureLod(waterTileTex, zuv1, mipmap).rg;
    vec2 b = textureLod(waterTileTex, zuv2, mipmap).rg;
    vec2 c = textureLod(waterTileTex, zuv3, mipmap).rg;
    vec2 d = textureLod(waterTileTex, zuv4, mipmap).rg;
	return (a + b + c + d) * 0.25;*/
    //return heightwaterHI(uv * WaterScale * 0.0001) * vec2(1.0);
}
vec2 heightwaterX(vec2 uv, float mipmap){
    return heightwaterXO(uv, vec2(0.0), mipmap);
	//return textureLod(waterTileTex, uv * WaterScale * octavescale1, mipmap).rg +  pow(textureLod(waterTileTex, uv * WaterScale * octavescale1 * 0.03, mipmap +3.0).rg * 2.0, vec2(3.0)) * 1100.0;;
	return textureLod(waterTileTex, uv * WaterScale * octavescale1, mipmap).rg;//  + vec2(supernoise3dX(vec3(WaterScale.x * uv.x * 0.2 + Time* 0.2, WaterScale.y * uv.y * 0.3 + Time * 0.1, Time * 0.1)) * 4.0, 0.0) * 0.2 * (1.0 - smoothstep(0.0, maxmip - 4.0, mipmap));
	vec2 zuv1 = (uv * WaterScale * octavescale1) + vec2(Time * 0.001 * WaterSpeed);
	vec2 zuv2 = zuv1 - vec2(Time * 0.0014 * WaterSpeed);
	vec2 zuv3 = zuv1 + vec2(Time * 0.0017 * WaterSpeed);
	vec2 zuv4 = zuv1 - vec2(Time * 0.0018 * WaterSpeed);
    vec2 a = textureLod(waterTileTex, zuv1, mipmap).rg;
    vec2 b = textureLod(waterTileTex, zuv2, mipmap).rg;
    vec2 c = textureLod(waterTileTex, zuv3, mipmap).rg;
    vec2 d = textureLod(waterTileTex, zuv4, mipmap).rg;
	return (a + b + c + d) * 0.25;
    //return heightwaterHI(uv * WaterScale * 0.0001) * vec2(1.0);
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
#define waterdepth 2.0 * WaterWavesScale
