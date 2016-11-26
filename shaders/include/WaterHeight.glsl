
uniform float WaterHeight;
uniform vec3 Wind;
uniform vec2 WaterScale;
float octavescale1 = 0.0001;
float mipmap1 = 0.0;
float maxmip = textureQueryLevels(waterTileTex);
float hashZ( float n ){
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
	return mix(mix(	mix( hashZ(n+0.0), hashZ(n+1.0),f.x),
			mix( hashZ(n+h1), hashZ(n+h1+1.0),f.x),f.y),
		   mix(	mix( hashZ(n+h2), hashZ(n+h2+1.0),f.x),
			mix( hashZ(n+h3), hashZ(n+h3+1.0),f.x),f.y),f.z);
}

float supernoise(vec2 x){
    float a = noise2X(x, 55.0, 92.0, 0.0);
    float b = noise2X(x + 0.5, 133.0, 326.0, 0.5);
    return (a * b);
}


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


vec2 heightwaterX(vec2 uv, float mipmap){
	vec2 zuv1 = (uv * WaterScale * 0.0001) + vec2(Time * 0.01 * WaterSpeed);
	vec2 zuv2 = zuv1 - vec2(Time * 0.014 * WaterSpeed);
	vec2 zuv3 = zuv1 + vec2(Time * 0.017 * WaterSpeed);
	vec2 zuv4 = zuv1 - vec2(Time * 0.018 * WaterSpeed);
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
float heightwater(vec2 uv){
    return heightwaterD(uv, mipmap1 * 1.0).r;
}
#define WaterLevel WaterHeight
#define waterdepth 80.0 * WaterWavesScale