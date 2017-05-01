

#include Shade.glsl
#include noise3D.glsl
#include Constants.glsl
#include PlanetDefinition.glsl
#include ProceduralValueNoise.glsl


float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{ return dot(point - origin, normal) / dot(direction, normal); }

vec3 getAtmosphereForDirection(vec3 dir, float roughness){
    float levels = max(0, float(textureQueryLevels(atmScattTex)));
    float mx = log2(roughness*1024.0+1.0)/log2(1024.0);
    float mlvel = mx * levels;
    return textureLod(atmScattTex, dir, mlvel).rgb;
}

#define ssnoise(a) (snoise(a) * 0.5 + 0.5)

/*
#define xdnoise(a) ssnoise(a + ssnoise(a * 0.5) * 0.5)

float hcl = 0.0;
float cloudsDensity3DXXXX(vec3 pos){
    hcl = 1.0 - smoothstep(CloudsFloor, CloudsCeil, length(vec3(0, planetradius, 0) + pos) - planetradius);
    //pos += (getWind(pos * 0.0005  * WindBigScale) * WindBigPower * 1.3 + getWind(pos * 0.00155  * WindSmallScale) * WindSmallPower * 0.1) * 2000.0 + CloudsOffset * 100.0;
    pos += (getWind(pos * 0.0005  * WindBigScale) * WindBigPower * 1.3 + getWind(pos * 0.00155  * WindSmallScale) * WindSmallPower * 0.1) * 2000.0 + CloudsOffset * 100.0;
    float partitions = ssnoise(pos * 0.00001 * vec3(FBMINITSCALE, FBMINITSCALE, FBMINITSCALE));
    float partitions2 = ssnoise(pos * 0.00004 * vec3(FBMINITSCALE2, FBMINITSCALE2, FBMINITSCALE2));
    float aza = smoothstep(0.0, 0.1, hcl) * (1.0 - smoothstep(0.8, 1.0, hcl));
    partitions = (partitions * partitions2) ;
    float fao1 = FBMO1 * 0.1;
    float fao2 = FBMO2 * 0.1;
    //float localaberations = mix(1.0, xdnoise(pos * 0.0001 * FBMS1) * fao1 + 1.0 * (1.0 - fao1), hcl);
    float localaberations = ssnoise(pos * 0.0001 * FBMS1) * fao1 + 1.0 * (1.0 - fao1);
    //float localaberations2 = mx(1.0, xdnoise(pos * 0.0001 * FBMS1) * fao1 + 1.0 * (1.0 - fao1), hcl);
    float localaberations2 = ssnoise(pos * 0.001 * FBMS2).x * fao2 + 1.0 * (1.0 - fao2);
    float density = partitions * localaberations * localaberations2;
    return smoothstep(
        CloudsThresholdLow,
        CloudsThresholdHigh,
        density * aza);
} */
#define fbm fbm_alu

#define xsupernoise3d(a) abs(0.5 - supernoise3d(a))*2.0
float fbmLOW(vec3 p){
   // p *= 0.1;
    p *= 0.011;
    float a = 0.0;
    float w = 0.66;
    for(int i=0;i<4;i++){
        //p += noise(vec3(a));
        a += xsupernoise3d(p) * w;
        w *= 0.5;
        p = p * 3.0  ;
    }
    return a;// + noise(p * 100.0) * 11;
}

float fbmFronts(vec3 p){
   // p *= 0.1;
    p *= 0.011;
    float a = 0.0;
    float w = 0.5;
    for(int i=0;i<4;i++){
        //p += noise(vec3(a));
        a += xsupernoise3d(p) * w;
        w *= 0.45;
        p = p * 3.0 ;
    }
    return smoothstep(0.1, 0.5, a);// + noise(p * 100.0) * 11;
}

float fbmHI(vec3 p){
   // p *= 0.1;
    p *= 0.011;
    float a = 0.0;
    float w = 0.5;
    for(int i=0;i<5;i++){
        //p += noise(vec3(a));
        a += xsupernoise3d(p) * w;
        w *= 0.5;
        p = p * 3.0  ;
    }
    return a;// + noise(p * 100.0) * 11;
}
vec3 getWind(vec3 p){
    float w = supernoise3d(p);
    return (vec3(
        sin(w * 10.0),
        sin(-w * 11.40),
        cos(w * 9.120)
    ) * 2.0 - 1.0);// * (supernoise3d(p * 0.1) * 0.5 + 0.5);
}
float getHeightOverSea(vec3 p){
    vec3 atmpos = vec3(0.0, planetradius, 0.0) + p;
    return length(atmpos) - planetradius;
}
float cloudsfrontsfbm(vec3 p){
	float a = 0.0;
    float w = 1.0;
    float wc = 0.0;
	for(int i=0;i<1;i++){
        //p += noise(vec3(a));
		a += clamp(2.0 * abs(0.5 - (supernoise3dX(p))) * w, 0.0, 1.0);
		wc += w;
        w *= 0.5;
		p = p * 3.0;
	}
	return a / wc;// + noise(p * 100.0) * 11;
}
float getFronts(vec3 p){
    p *= 0.000011 * FBMSCALE;
    //p.z += Time;
	return pow(cloudsfrontsfbm(p), 2.0);
}
float globalFrontsMultiplier = 0.0;
#define xsupernoise3dx(a) abs(0.5 - supernoise3d(a))*2.0
float fbmHxI(vec3 p){
   // p *= 0.1;
    float a = 0.0;
    float w = 0.5;
    for(int i=0;i<8;i++){
	float x = xsupernoise3dx(p);
        a += x * w;
        p = p * (2.9 + x * w * 0.006 );
        w *= 0.5;
    }
    return a;
}

float cloudsDensity3D(vec3 pos){
    vec3 ps = pos;

    vec3 p = ps * 0.009;
     p += CloudsOffset ;
//    p += getWind(p * WindBigScale * 0.1) * WindBigPower * 0.1;

	vec3 timev = vec3(Time*0.4, Time * 0.2, 0.0);
	vec3 windpos = p  + timev * 5.0 ;
    //vec3 thp = p * 10.021;
    //float density = fbmHxI(thp * 0.25 + fbmHxI( thp   * 0.15 )  * 0.15 );// * smoothstep(0.3, 0.7, supernoise3d(p*0.0008 ));
    float density = fbmHI(windpos + vec3(fbmHI(p) * 110.0, 0.0, fbmHI(p + 1000.0) * 110.0)) * smoothstep(0.3, 0.7, supernoise3d(windpos*0.0008 ));

    float measurement = (CloudsCeil - CloudsFloor) * 0.5;
    float mediana = (CloudsCeil + CloudsFloor) * 0.5;
    float h = getHeightOverSea(pos);
    float mlt = (( 1.0 - (abs( h - mediana ) / measurement )));
    float init = smoothstep(CloudsThresholdLow, CloudsThresholdHigh, density * mlt);
    return init;
}

float rand2s(vec2 co){
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}
float rand2sTime(vec2 co){
    co *= Time;
    return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

float hitfloorX = 0.0;
Sphere sphere1;
Sphere sphere2;
vec3 CAMERA = vec3(0.0, 1.0, 0.0);
float weightshadow = 1.1;
float internalmarchconservativeCoverageOnly(int s, vec3 p1, vec3 p2, float weight){
    int stepcount = s;
    const float stepsize = 1.0 / float(stepcount);
    float iter = 0.0;
    float rd = rand2sTime(UV) * stepsize;
    float coverageinv = 1.0;
    float linear = distance(p1, mix(p1, p2, stepsize));
    float mult = stepsize;
    for(int i=0;i<stepcount;i++){
        vec3 pos = mix(p1, p2, iter + rd);
        float clouds = cloudsDensity3D(pos) * weight;
        coverageinv -= clouds;
        iter += stepsize;
        if(coverageinv <= 0.0) break;
    }

    float r1 = clamp(coverageinv, 0.0, 1.0);

    vec3 xdir = normalize(p2 - p1);
    vec3 xpos = p1 + xdir * 500.0;
    //float refinedshadow = visibilityspecial(normalize(p1 - CAMERA), normalize(xpos - CAMERA), length(p1 - CAMERA), length(xpos - CAMERA));
    p2 = xpos;

    iter = 0.0;
    coverageinv = 1.0;
    linear = distance(p1, mix(p1, p2, stepsize));
    mult = weight * stepsize;
    for(int i=0;i<stepcount;i++){
        vec3 pos = mix(p1, p2, iter + rd);
        float clouds = cloudsDensity3D(pos) * weight;
        coverageinv -= clouds;
        iter += stepsize;
        if(coverageinv <= 0.0) break;
    }

    return r1 *  clamp(coverageinv, 0.0, 1.0);
}
float internalmarchconservativeCoverageOnly1StepOnly(int s, vec3 p1, vec3 p2, float weight){
    int stepcount = s;
    const float stepsize = 1.0 / float(stepcount);
    float iter = 0.0;
    float rd = rand2sTime(UV) * stepsize;
    float coverageinv = 1.0;
    float linear = distance(p1, mix(p1, p2, stepsize));
    float mult = stepsize;
    for(int i=0;i<stepcount;i++){
        vec3 pos = mix(p1, p2, iter + rd);
        float clouds = cloudsDensity3D(pos) * weight;
        coverageinv *= 1.0 - clouds;
        iter += stepsize;
//        if(coverageinv <= 0.0) break;
    }

    return clamp(coverageinv, 0.0, 1.0);
}


vec2 internalmarchconservative(vec3 p1, vec3 p2){
    int stepsmult = int(abs(CloudsFloor - CloudsCeil) * 0.001);
    int stepcount = 25 + stepsmult;
    float stepsize = 1.0 / float(stepcount);
    float rd = fract(rand2sTime(UV)) * stepsize;
    float c = 0.0;
    float w = 0.0;
    float coverageinv = 1.0;
    vec3 pos = vec3(0);
    float clouds = 0.0;
    float godr = 0.0;
    float godw = 0.0;
    float depr = 0.0;
    float depw = 0.0;
    float iter = 0.0;
    vec3 lastpos = p1;
    //depr += distance(CAMERA, lastpos);
    depw += 1.0;
    float linear = distance(p1, mix(p1, p2, stepsize));
    for(int i=0;i<stepcount;i++){
        pos = mix(p1, p2, iter + rd);
        clouds = cloudsDensity3D(pos);

        coverageinv -= clouds;
        depr += step(0.99, coverageinv) * distance(lastpos, pos);
        //if(coverageinv <= 0.0) break;
        lastpos = pos;
        iter += stepsize;
        //rd = fract(rd + iter * 124.345345);
    }
    if(coverageinv > 0.99) depr = 0;
    float cv = 1.0 - clamp(coverageinv, 0.0, 1.0);
    return vec2(cv, depr);
}


float visibility(vec3 dir1, vec3 dir2, float d1, float d2){
    float v = 1.0;
    float iter = 0.0;
    float stepsize = 1.0 / 16.0;
    float rd = stepsize * rand2s(UV * Time);
    iter += stepsize;
    for(int i=0;i<16;i++){
        vec3 md = mix(dir1, dir2, iter + rd);
        float dd = mix(d1, d2, iter + rd);
        float dx = textureLod(mainPassTex, md, 0.0).g + hitfloorX;
        if(dx < dd) return 0.0;
        iter += stepsize;
    }
    return v;
}

float smoothstepforao = 1.0 - smoothstep(0.0, 0.3, abs(CloudsThresholdLow - CloudsThresholdHigh));

float visibilityspecial(vec3 dir1, vec3 dir2, float z){
    float v = 0.0;
    float iter = 0.0;
    float stepsize = 1.0 / 8.0;
    float rd = stepsize * rand2s(UV * Time);
    iter += stepsize;
    vec3 atmorg = vec3(0,planetradius,0) + CAMERA;
    float d1 = textureLod(coverageDistTex, dir1, 1.0).g + hitfloorX;
    Ray r = Ray(atmorg, dir2);
    float xhitfloorXx = rsi2(r, sphere1);
    float d2 = z;

    vec3 raydir = normalize((dir2 * d2) - (dir1 * d1));
    for(int i=0;i<8;i++){
        vec3 md = normalize(mix(dir1, dir2, iter + rd));
        float dd = mix(d1, d2, iter + rd);
        Ray r = Ray(atmorg, md);
        float xhitfloorX = rsi2(r, sphere1);
        vec2 dx = textureLod(coverageDistTex, md, 1.0).rg;
        v += smoothstep(0.0, 1000.0, dx.x * max(0.0, dd - (dx.y + xhitfloorX)));
    }
    v *= stepsize;
    //v /= 9.0;
    return 1.0 - v;
}


float smart_inverse_dot(float dt, float coeff){
    return 1.0 - (1.0 / (1.0 + dt * coeff));
}

float hash1x = UV.x + UV.y + Time;
vec3 randdir(){
    float x = rand2s(UV * hash1x);
    hash1x += 0.5451;
    float y = rand2s(UV * hash1x);
    hash1x += 0.62123;
    float z = rand2s(UV * hash1x);
    hash1x += 0.4652344;
    return (vec3(
        x, y, z
    ) * 2.0 - 1.0);
}

float getAO(int s, vec3 pos, float randomization, float weight){
    //vec3 dir = normalize(dayData.sunDir);
    //vec3 dir = normalize(dayData.sunDir + randdir() * randomization);
    vec3 dir = normalize(dayData.sunDir + randdir() * randomization);
    Ray r = Ray(vec3(0,planetradius ,0) +pos, dir);
    Ray r2 = Ray(vec3(0,planetradius ,0) +pos, dir);
    float hitplanet = rsi2(r2, planet);
    //if(hitplanet > 0.0) return 0.0;
    float hitceil = rsi2(r, sphere2);
    float hitfloor = rsi2(r, sphere1);
    vec3 posceil = pos + dir * hitceil;
    vec3 posfloor = pos + dir * hitfloor;
    if(hitfloor > 0.0 && hitceil > 0.0 && hitfloor < hitceil) return internalmarchconservativeCoverageOnly1StepOnly(s, pos, posceil, weight);
    if(hitfloor > 0.0 && hitceil > 0.0 && hitfloor > hitceil) return internalmarchconservativeCoverageOnly1StepOnly(s, pos, posceil, weight);
    if(hitfloor > 0.0 && hitceil <= 0.0) return internalmarchconservativeCoverageOnly1StepOnly(s, pos, posfloor, weight);
    if(hitfloor <= 0.0 && hitceil > 0.0) return internalmarchconservativeCoverageOnly1StepOnly(s, pos, posceil, weight);
    return internalmarchconservativeCoverageOnly1StepOnly(s, pos, posceil, weight);
}


float shadows(){
    vec3 viewdir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    vec3 atmorg = vec3(0,planetradius,0) + CAMERA;
    Ray r = Ray(atmorg, viewdir);
    sphere1 = Sphere(vec3(0), planetradius + CloudsFloor);
    sphere2 = Sphere(vec3(0), planetradius + CloudsCeil);
    hitfloorX = rsi2(r, sphere1);
    vec2 data = texture(coverageDistTex, viewdir).rg;
   // hash1x = rand2s(UV * vec2(Time, Time));
    vec3 hitman = CAMERA + viewdir * (data.g + hitfloorX);
    planet = Sphere(vec3(0), planetradius);
//    float mx1  = clamp(0.0, 1.0, MieScattCoeff * 0.2) * 0.5 + 0.5;
    weightshadow = 1.0;
    float sun = getAO(16, hitman, 0.19334, CloudsDensityScale * 0.1);// + (0.5 + max(0.0, (getAO(hitman, 1.0) * 2.0 - 1.0 ))) * 0.2 + getAO(hitman, 0.0);
    //weightshadow  = 0.000;//1 * CloudsDensityScale;
   // float sss =  getAO(hitman, 0.6, CloudsDensityScale  );
    return sun;
    //return sun * 0.8 + sss * 0.2;
}

#include ResolveAtmosphere.glsl
float globalAoOutput = 0.0;
vec3 getCloudsAL(vec3 dir, float val){
    //float val = shadows();
    //return vec3(val * 1110.0);
    val = max(0.0, val);
    vec2 asd = textureLod(coverageDistTex, dir, 1.0).rg;
    if(asd.r == 0) return vec3(0.0);
    float center = asd.g + hitfloorX;
    vec3 c = CAMERA;
    vec3 point = c + dir * center;

    vec3 sum = vec3(0.0);
    float r = Time;
    float dsr = max(0.0, dot(dayData.sunDir, dir));
    float vdt = max(0.0, dot(dayData.sunDir, VECTOR_UP));
    float vxdt = max(0.0, dot(dir, VECTOR_UP));
    float vdt2 = 1.0 - vdt;
    vec2 uv = UV + vec2(Rand1, Rand2);
    float rd = rand2s(uv) * 12.1232343456;
    float mult = 1.0;//mix(sqrt(dot(dayData.sunDir, dir) * 0.5 + 0.5), 1.0, vdt) + 0.02;
    float ao = 0.0;
    vec3 SunC = getSunColorDirectly(0.0);//* max(0.01, smart_inverse_dot(vdt, 6.0));
    for(int i=0;i<111;i++){
        float x = rand2s(uv + rd) * 2.0 - 1.0;
        //rd *= 2.7897;
        uv.x += 1.46456;
        float y = rand2s(uv + rd);
        //rd *= 1.6271255;
        uv.x += 1.234234;
        float z = rand2s(uv + rd) * 2.0 - 1.0;
        //rd *= 1.234211;
        uv.x += 1.567567;
        rd += 1.234;
        vec3 px = normalize(vec3(x, y, z));
        vec3 p = point + px * 14000.0;
        vec3 newdir = normalize(p - CAMERA);
        float v = 0.5 + 0.5 * visibilityspecial(dir, newdir, distance(CAMERA, p));
        //float v = internalmarchconservativeCoverageOnly1StepOnly(22, point, p, 1.0);
        ao += v;
        sum += normalize(textureLod(atmScattTex, px, 2.0).rgb + vec3(dsr))  * v;// * pow(vdt2, 4.0);
    }
    ao /= 111.0;
    globalAoOutput = ao ;
    float sao = ao;//;
    float saomoon = ao;

    //float sss_real = getAO(2, hitman, 8.0 * (CloudsThresholdHigh - CloudsThresholdLow), CloudsDensityScale);
    //float AOSky = 1.0 - AOGround;//getCloudsAO(dir, 1.0);

    float SunDT = max(0.0, dot(dayData.sunDir, VECTOR_UP) * 0.9 + 0.1);
    //float coverageSun = getAO(20, point, 1.0, CloudsDensityScale * 1.0);
//    return sum;
    //sao *= 1.0 + pow(caa, 2.0) * 1.0 * pow(max(0.0, dot(dir, dayData.sunDir)) * max(0.0, dot(dir, VECTOR_UP)), 5.0);
    //sao = mix(11.0, sao, max(0.0, 1.0 - 1.0 / (12.0 * max(0.0, dot(dir, VECTOR_UP)))));

    //saomoon = mix(11.0, saomoon, max(0.0, 1.0 - 1.0 / (12.0 * max(0.0, dot(dir, VECTOR_UP)))));
    saomoon *= max(0.0, 1.0 - 1.0 / (6.0 * max(0.0, dot(dayData.moonDir, VECTOR_UP))));
    //float dshadow = getAODIR(point, px, 113.0);
    float a = max(0.0, dot(dayData.sunDir, VECTOR_UP));
    vec3 sss = (1.0 - ao) * SunC * (smart_inverse_dot(dsr, 29.0)) * val * val;
    vec3 grounddiffuse = (ao * ao) * SunC * vdt* vdt * vdt  * 1.6;
    vec3 atm_radiance = textureLod(atmScattTex, VECTOR_UP, textureQueryLevels(atmScattTex).x - 1.0).rgb * ao;//smoothstep(CloudsFloor, CloudsCeil, getHeightOverSea(point) * 0.5 + 0.5) ;

    float coverage =  smoothstep(0.464, 0.6, CloudsThresholdLow);
  // return vec3(sao) ;
  //return vec3(ao);
    vec3 result =   vec3(0.0);// * max(0.0, 1.0 - 1.0 / (1.0 + 6.0 * max(0.0, dot(dir, dayData.moonDir))));// + sao * 3.0 + daox * 0.1 ;
    result += (SunC * 0.3 * val * (1.0 + 1.0 * (1.0 - smart_inverse_dot(1.0-dsr, 15.0))));
    result += sum / 111.0 ;
    //return vec3(ao);
//return vec3( 200 *  (smart_inverse_dot(vdt, 64.0)));
    return result;
    //return vec3(0.0) +  sao * 3.0;// + daox * 0.1 ;
}
#define intersects(a) (a >= 0.0)

vec2 raymarchCloudsRay(){
    vec3 viewdir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    vec3 atmorg = vec3(0,planetradius,0) + CAMERA;
    Ray r = Ray(atmorg, viewdir);
    vec3 campos = CAMERA;

    sphere1 = Sphere(vec3(0), planetradius + CloudsFloor);
    sphere2 = Sphere(vec3(0), planetradius + CloudsCeil);

    float hitfloor = rsi2(r, sphere1);
    float hitceil = rsi2(r, sphere2);

    return internalmarchconservative(campos + viewdir * hitfloor, campos + viewdir * hitceil);
}
