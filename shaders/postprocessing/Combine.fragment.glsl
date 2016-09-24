#version 430 core

#include PostProcessEffectBase.glsl

layout(binding = 24) uniform sampler2DArray CSMTex;

#define MAX_CSM_LAYERS 12
uniform mat4 CSMVPMatrices[MAX_CSM_LAYERS];
uniform float CSMRadiuses[MAX_CSM_LAYERS];
uniform int CSMLayers;
layout(binding = 3) uniform samplerCube skyboxTex;
layout(binding = 5) uniform sampler2D directTex;
layout(binding = 6) uniform sampler2D alTex;
layout(binding = 16) uniform sampler2D aoxTex;
layout(binding = 20) uniform sampler2D fogTex;

layout(binding = 25) uniform samplerCube coverageDistTex;
layout(binding = 26) uniform samplerCube shadowsTex;
layout(binding = 27) uniform samplerCube skyfogTex;

uniform int UseAO;
uniform float T100;
uniform float T001;

#define CLOUD_SAMPLES 18
#define CLOUDCOVERAGE_DENSITY 50    
#include Atmosphere.glsl
float rdhash = 0.453451 + Time;
vec2 randpoint2(){
    float x = rand2s(UV * rdhash);
    rdhash += 2.1231255;
    float y = rand2s(UV * rdhash);
    rdhash += 1.6271255;
    return vec2(x, y) * 2.0 - 1.0;
}
vec3 randpoint3(){
    float x = rand2s(UV * rdhash);
    rdhash += 2.1231255;
    float y = rand2s(UV * rdhash);
    rdhash += 1.6271255;
    float z = rand2s(UV * rdhash);
    rdhash += 1.6271255;
    return vec3(x, y, z) * 2.0 - 1.0;
}
/*
float aaoo(){
    vec2 uv = reverseViewDir();
    float center = texture(cloudsCloudsTex, uv).b;
    float aoc = 0;
    for(int i=0;i<32;i++){
        vec2 rdp = randpoint2() * 0.01;
        float there = texture(cloudsCloudsTex, uv + rdp).b;
        float w = 1.0 - smoothstep(1000.0, 10000, center - there);
        aoc += w * clamp(center - there, 0.0, 1000.0) * 0.001;
    }
    return pow(1.0 - aoc / 32.0, 16.0);
}    */
float roughtomipmap(float roughness, sampler2D txt){
    //roughness = roughness * roughness;
    float levels = max(0, float(textureQueryLevels(txt)));
    float mx = log2(roughness*512+1)/log2(512);
    return mx * levels;
}
vec4 smartblur(vec3 dir, float roughness){
    float levels = max(0, float(textureQueryLevels(cloudsCloudsTex)));
    float mx = log2(roughness*1024+1)/log2(1024);
    float mlvel = mx * levels;
    vec4 ret = vec4(0);
    ret.xy = textureLod(coverageDistTex, dir, mlvel).rg;
    ret.z = textureLod(shadowsTex, dir, mlvel).r;
    ret.w = textureLod(skyfogTex, dir, mlvel).r;
    return ret;
   // return textureLod(cloudsCloudsTex, dir, mlvel).rgba;
    vec4 centervals = textureLod(cloudsCloudsTex, dir, mlvel).rgba;
    vec4 centerval = vec4(0);
    vec2 center = textureLod(cloudsCloudsTex, dir, mlvel).rg;
    float aoc = 0;
    float blurrange = 0.001;
    for(int i=0;i<12;i++){
        vec3 rdp = normalize(dir + randpoint3() * blurrange);
        float there = textureLod(cloudsCloudsTex, rdp, mlvel).b;
        float w = pow( 1.0 - abs(there - centervals.b), 2.0);
        centerval += w* textureLod(cloudsCloudsTex, rdp, mlvel).rgba;
        aoc += w;
    }
    centerval /= aoc;
    return vec4(centerval.r, centerval.g, centerval.b, centerval.a);
}



float fogatt(float dist){
    dist *= 0.000015;
    return min(1.0, (dist * dist) );
}

#define wassnoise(a) pow((snoise(a) * 0.5 + 0.5), 1.0)
vec3 octaveN(vec2 a, float esp){
    vec2 zxpos = a * esp;
    float h1 = snoise(vec3(zxpos.xy, Time * 0.1));
    float h2 = wassnoise(vec3(zxpos.xy, Time * 0.1));
    float h3 = snoise(vec3(zxpos.xy, Time * 0.1));
    return normalize(vec3(h1, h2, h3));
}

float hashX( float n ){
    return fract(sin(n)*758.5453);
}

float noiseX( in vec3 x ){
    vec3 p = floor(x);
    vec3 f = fract(x); 
    float n = p.x + p.y*57.0 + p.z*800.0;
    float res = mix(mix(mix( hashX(n+  0.0), hashX(n+  1.0),f.x), mix( hashX(n+ 57.0), hashX(n+ 58.0),f.x),f.y),
    mix(mix( hashX(n+800.0), hashX(n+801.0),f.x), mix( hashX(n+857.0), hashX(n+858.0),f.x),f.y),f.z);
    return res;
}
float noise2X( in vec2 x ){
    vec2 p = floor(x);
    vec2 f = fract(x); 
    float n = p.x + p.y*57.0;
    float res = 
        mix (
            mix (
                hashX (
                    n + 0.0
                ), hashX (
                    n + 1.0
                ), f.x
            ), mix (
                hashX (
                    n + 57.0
                ), hashX (
                    n + 58.0
                ), f.x
            ), f.y
        );
    return res;
}
#define ssin(a) (smoothstep(0,3.1415,a*3.1415) * 2.0 - 1.0) 
#define snoisesin(a) pow(1.0 - (abs(noise2X(a) - 0.5) * 2.0), 6.0)
#define snoisesinpow(a,b) pow(1.0 - (abs(noise2X(a) - 0.5) * 2.0), b)
#define snoisesinpowXF(a,b) (1.0 - pow((abs(snoise(a))), b))
float heightwaterHI(vec2 pos, int s){
    pos *= 0.009;
    pos *= vec2(NoiseOctave2, NoiseOctave3);
    float res = 0.0;
    float w = 0.0;
    float wz = 1.0;
    float chop = 3.0;
    float tmod = 260.1;
    for(int i=0;i<s;i++){
        //vec2 t = vec2(0, tmod * T001);
        vec2 t = vec2(noise2X(pos)*2.0 +  tmod * T001);
        res += wz * snoisesinpow(pos + t.yx, chop);
        res += wz * snoisesinpow(pos - t.yx, chop);
        w += wz * 2;
        wz *= 0.4;
        pos *= 2.8;
        tmod *= 1.8;
    }
    return res / w;// * 0.5 + (sin((noise2X(pos.xx * 0.001) * 0.5 + 0.5) * pos.x*0.01 + Time) * 0.5 + 0.5);
}
#define noise2A(a) noise(a + noise(a*vec3(1.0, 1.0, 0.5)))

float fbmlow(vec3 pos)
{
	return (noise2A(pos) * 0.5 +
		noise2A(pos * 2.0 ) * 0.25 +
		noise2A(pos * 4.0) * 0.125 +
		noise2A(pos * 8.0) * 0.0625 +
		noise2A(pos * 16.0) * 0.03125);
}


vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
  return mod289(((x*34.0)+1.0)*x);
}

float snoise2d(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}
#define noise2XZ(a) (noise2X(a) * 0.5 + 0.5)
float heightwater(vec2 pos, int s, float timeshift){
    pos *= 0.06;
    pos *= vec2(NoiseOctave2, NoiseOctave3);
    float res = 0.0;
    float w = 0.0;
    float wz = 1.0;
    float chop = 1.0;
    float tmod = 94.1;
    float T = Time + timeshift;
    float t001 = T * 0.001;
    for(int i=0;i<s;i++){   
        vec2 t = vec2(tmod * t001);
        res += wz * sin(snoise2d(pos + t.yx) * 2.0 + T * 3.0 + pos.x);
        w += wz;
        wz *= 0.4;
        pos *= 1.8;
        tmod *= 1.8;
    }
    return (res / w) * 0.5 + 0.5;// * 0.5 + (sin((noise2X(pos.xx * 0.001) * 0.5 + 0.5) * pos.x*0.01 + Time) * 0.5 + 0.5);
}


vec3 hitpos = vec3(0);
float hitdistx = 0;
#define waterdepth 1.0 * WaterWavesScale
#define WATER_SAMPLES_LOW 1
#define WATER_SAMPLES_HIGH 4
vec3 normalx(vec3 pos, float e){
    hitpos = pos;
    hitdistx = distance(CameraPosition, pos);
    vec2 ex = vec2(e, 0);
    vec3 a = vec3(pos.x, heightwater(pos.xz, WATER_SAMPLES_HIGH, 0.0) * waterdepth, pos.z);    
    vec3 b = vec3(pos.x + e, heightwater(pos.xz + ex.xy, WATER_SAMPLES_HIGH, 0.0) * waterdepth, pos.z);
    vec3 c = vec3(pos.x, heightwater(pos.xz - ex.yx, WATER_SAMPLES_HIGH, 0.0) * waterdepth, pos.z - e);      
    vec3 normal = (cross(normalize(a-b), normalize(a-c)));
    return normalize(normal).xyz;// + 0.1 * vec3(snoise(normal), snoise(-normal), snoise(normal.zyx)));
}
vec3 normalhix(vec3 pos, float e){
    vec2 ex = vec2(e, 0);
    vec3 a = vec3(pos.x, heightwater(pos.xz, WATER_SAMPLES_HIGH, 0.0) * waterdepth, pos.z);    
    vec3 b = vec3(pos.x + e, heightwater(pos.xz + ex.xy, WATER_SAMPLES_HIGH, 0.0) * waterdepth, pos.z);
    vec3 c = vec3(pos.x, heightwater(pos.xz - ex.yx, WATER_SAMPLES_HIGH, 0.0) * waterdepth, pos.z - e);      
    vec3 normal = (cross(normalize(a-b), normalize(a-c)));
    return normalize(normal).xyz;// + 0.1 * vec3(snoise(normal), snoise(-normal), snoise(normal.zyx)));
}
vec3 normalxlow(vec3 pos, float e){
    vec2 ex = vec2(e, 0);
    vec3 a = vec3(pos.x, heightwater(pos.xz, WATER_SAMPLES_LOW, 0.0) * waterdepth, pos.z);    
    vec3 b = vec3(pos.x + e, heightwater(pos.xz + ex.xy, WATER_SAMPLES_LOW, 0.0) * waterdepth, pos.z);
    vec3 c = vec3(pos.x, heightwater(pos.xz - ex.yx, WATER_SAMPLES_LOW, 0.0) * waterdepth, pos.z - e);      
    vec3 normal = (cross(normalize(a-b), normalize(a-c))); ;
    return normalize(normal).xyz;// + 0.1 * vec3(snoise(normal), snoise(-normal), snoise(normal.zyx)));
}

vec3 raymarchwaterImpl(vec3 upper, vec3 lower, int stepsI){
    float stepsize = 1.0 / stepsI;
    float iter = 0;
    float rdo = rand2s(UV * Time);
    float rd = stepsize * rdo;
    float maxdist = length(currentData.normal) < 0.07 ? 999998.0 : currentData.cameraDistance;
    for(int i=0;i<stepsI + 1;i++){
        vec3 pos = mix(upper, lower, iter + rd);
      //  pos.x += iter * 30.0;
        float dst = distance(pos, CameraPosition);
        float h = heightwater(pos.xz, WATER_SAMPLES_LOW, 0.0);
        if(h > 1.0 - (iter + rd) || dst > maxdist) {
            return normalx(pos, 0.1);
        }
        iter += stepsize;
    }
    return normalx(upper, 0.1);
}
vec3 raymarchwater(vec3 upper, vec3 lower, int si){
    return raymarchwaterImpl(upper, lower, si);
}

vec3 raymarchwaterLOW3(vec3 upper, vec3 lower){
    vec3 n = normalxlow(upper, 0.1);
    hitpos = upper;
    hitdistx = distance(CameraPosition, hitpos);
    return n;
}
vec3 raymarchwaterHIGH3(vec3 upper, vec3 lower){
    vec3 n = normalx(upper, 0.1);
    //hitpos = upper;
    //hitdistx = distance(CameraPosition, hitpos);
    return n;
}

#define WaterLevel (0.01+ 10.0 * NoiseOctave6)

float underwaterPercentage(vec3 pos){
    float rf = pos.y;
    float u1 = max(0.0, heightwater(pos.xz, WATER_SAMPLES_LOW, -0.2) * waterdepth + WaterLevel - rf);
    float u2 = max(0.0, heightwater(pos.xz, WATER_SAMPLES_LOW, -0.8) * waterdepth + WaterLevel - rf);
    float u3 = max(0.0, heightwater(pos.xz, WATER_SAMPLES_LOW, -1.6) * waterdepth + WaterLevel - rf);
    float u4 = max(0.0, heightwater(pos.xz, WATER_SAMPLES_LOW, -2.8) * waterdepth + WaterLevel - rf);
    return max(0.0, 1.0 - (u1 + u2 * 0.75 + u3 * 0.5 + u4 * 0.33) * 1.8);
}

#define LOD3 422.0

vec2 projectvdao(vec3 pos){
    vec4 tmp = (VPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}
//uniform mat4 CSMPMatrices[MAX_CSM_LAYERS];
//uniform float CSMFarplanes[MAX_CSM_LAYERS];
//uniform float CSMRadiuses[MAX_CSM_LAYERS];
//uniform vec3 CSMCenter;
//uniform mat4 CSMVMatrix;

vec3 CSMProject(vec3 pos, int layer){
    vec4 tmp = ((CSMVPMatrices[layer]) * vec4(pos, 1.0));
    //tmp.xy /= tmp.w;
    return (tmp.xyz);
}
float rdhasha = Time * 0.1;
vec3 randpointx(){
    float x = rand2s(UV * rdhasha);
    rdhasha += 2.1231255;
    float y = rand2s(UV * rdhasha);
    rdhasha += 1.6271255;
    return vec3(x, y, 0.5) * 2.0 - 1.0;
}
float giscale = 1.0; 
float blurshadowabit(float blurscale, int samples, vec3 uv, float d){
    float v = 0.0;
    const float w = 1.0 / float(samples);
    for(int i=0;i<samples;i++){
        v += smoothstep(0.0, 0.0032, d - texture(CSMTex, uv + randpointx() * 0.24115 * giscale * blurscale).r);
      //  v += 1.0 - max(0, sign(d - texture(CSMTex, uv + randpointx() * 0.00115).r));
    }
    return v * w;
}   
float blurshadowabitGI(vec3 uv, float d){
    float v = 0.0;
    const float w = 1.0 / 24.0;
    for(int i=0;i<24;i++){
        v += smoothstep(0.0, 0.0022, d - texture(CSMTex, uv + randpointx() * 11.6115 * giscale).r);
      //  v += 1.0 - max(0, sign(d - texture(CSMTex, uv + randpointx() * 0.00115).r));
    }
    return v * w;
}

float CSMQueryVisibility(vec3 pos){
//return CSMVPMatrices[0][0].x;
//return 1.0;
    for(int i=0;i<CSMLayers;i++) if(distance(pos, CameraPosition) < CSMRadiuses[i]){
        vec3 csmuv = CSMProject(pos, i);
       // csmuv.z *= -1;
        csmuv = csmuv * 0.5 + 0.5;
        float d = csmuv.z;
        if(csmuv.x >= 0.0 && csmuv.x < 1.0 && csmuv.y >= 0.0 && csmuv.y < 1.0 && csmuv.z >= 0.0 && csmuv.z < 1.0){
            giscale = 1.0 / CSMRadiuses[i];
            return 1.0 - blurshadowabit(1.0, 18, vec3(csmuv.x, csmuv.y, float(i)), d);// abs(csmuv.z - texture(CSMTex, csmuv).r) ;
        }
    }
    return 1.0;
}
float CSMQueryVisibilityVariable(vec3 pos, int samples, float blurscale){
//return CSMVPMatrices[0][0].x;
//return 1.0;
    for(int i=0;i<CSMLayers;i++) if(distance(pos, CameraPosition) < CSMRadiuses[i]){
        vec3 csmuv = CSMProject(pos, i);
       // csmuv.z *= -1;
        csmuv = csmuv * 0.5 + 0.5;
        float d = csmuv.z;
        if(csmuv.x >= 0.0 && csmuv.x < 1.0 && csmuv.y >= 0.0 && csmuv.y < 1.0 && csmuv.z >= 0.0 && csmuv.z < 1.0){
            giscale = 1.0 / CSMRadiuses[i];
            return 1.0 - blurshadowabit(blurscale, samples, vec3(csmuv.x, csmuv.y, float(i)), d);// abs(csmuv.z - texture(CSMTex, csmuv).r) ;
        }
    }
    return 1.0;
}
float CSMQueryVisibilityGI(vec3 pos){
//return CSMVPMatrices[0][0].x;
//return 1.0;
    for(int i=0;i<CSMLayers;i++) if(distance(pos, CameraPosition) < CSMRadiuses[i]){
        vec3 csmuv = CSMProject(pos, i);
       // csmuv.z *= -1;
        csmuv = csmuv * 0.5 + 0.5;
        float d = csmuv.z;
        if(csmuv.x >= 0.0 && csmuv.x < 1.0 && csmuv.y >= 0.0 && csmuv.y < 1.0 && csmuv.z >= 0.0 && csmuv.z < 1.0){
            giscale = 1.0 / CSMRadiuses[i];
            return 1.0 - blurshadowabitGI(vec3(csmuv.x, csmuv.y, float(i)), d);// abs(csmuv.z - texture(CSMTex, csmuv).r) ;
        }
    }
    return 1.0;
}
float CSMQueryVisibilitySimple(vec3 pos){
//return CSMVPMatrices[0][0].x;
    for(int i=0;i<CSMLayers;i++) if(distance(pos, CameraPosition) < CSMRadiuses[i]){
        vec3 csmuv = CSMProject(pos, i);
       // csmuv.z *= -1;
        csmuv = csmuv * 0.5 + 0.5;
        float d = csmuv.z;
        if(csmuv.x >= 0.0 && csmuv.x < 1.0 && csmuv.y >= 0.0 && csmuv.y < 1.0 && csmuv.z >= 0.0 && csmuv.z < 1.0){
          //  return 1.0 - blurshadowabit(vec3(csmuv.x, csmuv.y, float(i)), d);// abs(csmuv.z - texture(CSMTex, csmuv).r) ;
            return 1.0 - max(0, sign(d - texture(CSMTex, vec3(csmuv.x, csmuv.y, float(i))).r - 0.0012));// abs(csmuv.z - texture(CSMTex, csmuv).r) ;
        }
    }
    return 1.0;
}
float CSMReconstructDistance(vec3 pos){
//return CSMVPMatrices[0][0].x;
    for(int i=0;i<CSMLayers;i++){
        vec3 csmuv = CSMProject(pos, i);
       // csmuv.z *= -1;
        csmuv = csmuv * 0.5 + 0.5;
        float z = csmuv.z;
        if(csmuv.x >= 0.0 && csmuv.x < 1.0 && csmuv.y >= 0.0 && csmuv.y < 1.0 && csmuv.z >= 0.0 && csmuv.z < 1.0){
          float d = texture(CSMTex, vec3(csmuv.x, csmuv.y, float(i))).r;
          vec4 cp = vec4(csmuv.x, csmuv.y, d, 1.0) * 2.0 - 1.0;
          vec4 wp = (inverse(CSMVPMatrices[i]) * cp);
          return distance(wp.xyz / wp.w, pos);
        }
    }
    return 1.0;
}

float rand2sx(vec2 co){
        return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

#define MMAL_LOD_REGULATOR 1024
vec3 stupidBRDF(vec3 dir, float level, float roughness){
    vec3 aaprc = vec3(0.0);
    float xx=rand2s(UV);
    float xx2=rand2s(UV.yx);
    for(int x = 0; x < 14; x++){
        vec3 rd = vec3(
            rand2s(vec2(xx, xx2)),
            rand2s(vec2(-xx2, xx)),
            rand2s(vec2(xx2, xx))
        ) *2-1;
        vec3 displace = rd;
        vec3 prc = textureLod(skyboxTex, dir + (displace * 0.1 * roughness), level).rgb * 5.0;
        aaprc += prc;
        xx += 0.01;
        xx2 -= 0.02123;
    }
    return pow(aaprc / 14.0, vec3(2.4));
}

vec3 MMALSkybox(vec3 dir, float roughness){
    //roughness = roughness * roughness;
    float levels = max(0, float(textureQueryLevels(skyboxTex)) - 1);
    float mx = log2(roughness*MMAL_LOD_REGULATOR+1)/log2(MMAL_LOD_REGULATOR);
    vec3 result = pow(textureLod(skyboxTex, dir, mx * levels).rgb, vec3(2.4)) * 5;
    
    //return pow(result * 1.2, vec3(2.0));
    return result;
}

vec3 atmc = getAtmosphereForDirection(normalize(SunDirection), currentData.roughness) * 55.0 * normalize(SunDirection).y;

float getthatfuckingfresnel(float reflectivity, vec3 normal, vec3 cameraSpace, float roughness){
    vec3 dir = normalize(reflect(normalize(cameraSpace), normal));
    float base =  1.0 - abs(dot(normalize(normal), dir));
    float fresnel = (reflectivity + (1-reflectivity)*(pow(base, mix(5.0, 0.8, roughness))));
    float angle = 1.0 - base;
    return fresnel * (1.0 - roughness);
}

vec3 shadingMetalic(PostProceessingData data){
    float fresnelR = getthatfuckingfresnel(data.diffuseColor.r, data.normal, data.cameraPos, data.roughness);
    float fresnelG = getthatfuckingfresnel(data.diffuseColor.g, data.normal, data.cameraPos, data.roughness);
    float fresnelB = getthatfuckingfresnel(data.diffuseColor.b, data.normal, data.cameraPos, data.roughness);
    float fresnel = getthatfuckingfresnel(0.04, data.normal, normalize(data.cameraPos), data.roughness);
    vec3 newBase = vec3(fresnelR, fresnelG, fresnelB);
 //   return vec3(fresnel);
    float x = 1.0 - max(0, -dot(normalize(SunDirection), currentData.originalNormal));
    return shade(CameraPosition, newBase, data.normal, data.worldPos, data.worldPos + normalize(SunDirection) * 40.0, vec3(atmc),  max(0.004, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - currentData.roughness);
}

vec3 shadingNonMetalic(PostProceessingData data){
    float fresnel = getthatfuckingfresnel(0.04, data.normal, normalize(data.cameraPos), data.roughness);
    float x = 1.0 - max(0, -dot(normalize(SunDirection), currentData.normal));
    
    vec3 radiance = shade(CameraPosition, vec3(fresnel), data.normal, data.worldPos, data.worldPos + normalize(SunDirection) * 40.0, vec3(atmc) * currentData.diffuseColor, max(0.004, data.roughness), false) * mix(x, pow(x, 8.0), 1.0 - currentData.roughness);    
    
    vec3 difradiance = shadeDiffuse(CameraPosition, data.diffuseColor * (1.0 - fresnel), data.normal, data.worldPos, data.worldPos + normalize(SunDirection) * 40.0, vec3(atmc), 0.0, false) * x;
 //   return vec3(0);
    return radiance + difradiance ; 
}

vec3 MakeShading(PostProceessingData data){
    return mix(shadingNonMetalic(data), shadingMetalic(data), data.metalness);// * (UseAO == 1 ? texture(aoxTex, UV).r : 1.0);
}
vec3 MakeShadingGI(PostProceessingData data){
    return shadeInvariantGI(CameraPosition, data.diffuseColor, data.worldPos, data.worldPos + normalize(SunDirection) * 40.0, vec3(atmc));
}
vec3 MMAL(PostProceessingData data){
    vec3 reflected = normalize(reflect(data.cameraPos, data.normal));
    vec3 dir = normalize(mix(reflected, data.normal, data.roughness));
    
    float fresnel = fresnel_again(vec3(0.04), data.normal, data.cameraPos, data.roughness);
    float fresnelR = fresnel_again(vec3(data.diffuseColor.r), data.normal, data.cameraPos, data.roughness);
    float fresnelG = fresnel_again(vec3(data.diffuseColor.g), data.normal, data.cameraPos, data.roughness);
    float fresnelB = fresnel_again(vec3(data.diffuseColor.b), data.normal, data.cameraPos, data.roughness);
    vec3 newBase = vec3(fresnelR, fresnelG, fresnelB);
    
    vec3 metallic = vec3(0);
    vec3 nonmetallic = vec3(0);
    
    metallic += MMALSkybox(dir, data.roughness) * newBase;
    
    nonmetallic += MMALSkybox(dir, data.roughness) * fresnel;
    nonmetallic += MMALSkybox(dir, 1.0) *  data.diffuseColor * (1.0 - fresnel);
    
    return  MakeShading(currentData);
    
}

float causs(vec2 sp){
	vec2 p = sp*6.0 - vec2(125.0);
	vec2 i = p;
	float c = 1.0;
	float cc = fract(1.);
	int ccc = int(cc);
	
	float inten = 0.01;
	
		for (int n = 0; n < 4; n++) 
		{
			float t = Time * 0.2* (0.05 - (3.0 / float(n+1)));
			i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
			c += 1.0/length(vec2(p.x / (sin(i.x+t)/inten),p.y / (cos(i.y+t)/inten)));
		}
			c /= float(4);
			c = 1.5-sqrt(c);
	return c*c*c;
}

vec3 cloudsbydir(vec3 dir){
    bool underwater = CameraPosition.y < WaterLevel || WaterLevel + waterdepth * heightwater(CameraPosition.xz, WATER_SAMPLES_LOW, 0.0) > CameraPosition.y;   
    
    float planethit = intersectPlane(CameraPosition, dir, vec3(0.0, waterdepth + WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
    vec2 uvX = UV;
    if(underwater){
        float dst = planethit > 0.0 ? min(planethit, currentData.cameraDistance) : currentData.cameraDistance;
        dst = dst == 0.0 ? 99999.0 : dst;
        uvX = uvX + vec2(snoise2d(uvX * 3.0 + Time * 0.2), snoise2d(uvX.yx * 3.0 + Time * 0.2)) * min(0.006, dst * 0.0001);
        currentData = loadData(uvX);      
        dir = reconstructCameraSpaceDistance(uvX, 1.0);
        planethit = intersectPlane(CameraPosition, dir, vec3(0.0, waterdepth + WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
   }
    float dst = 0;
    float roughness = 0.0;
    vec3 basewaterclor = vec3(0);
    
    vec3 atmcolor = getAtmosphereForDirection(normalize(max(vec3(-1.0, 0.08, -1.0), SunDirection)), 0.0);
    
    float diminisher = max(0, 0.2 + 0.8 * dot(normalize(SunDirection), vec3(0,1,0)));
    float diminisherX = max(0,  dot(normalize(SunDirection), vec3(0,1,0)));
    vec3 litcolor2 = mix(vec3(8.0), atmcolor * 0.2, pow(1.0 - diminisher, 3.0));
    float cwpgi = 0;//CSMQueryVisibilityGI(currentData.worldPos) * 0.1 * diminisher * diminisher;
    float ln = length(currentData.normal);
    float fresnel = ln < 0.01 ? 1.0 : 0.0;
    float whites = (0);
    float doatmscatter = 1.0 ;
   //defres += getAtmosphereForDirection(currentData.worldPos, currentData.normal, normalize(SunDirection), currentData.roughness) * 0.5 * currentData.diffuseColor;
    Sphere planet0 = Sphere(vec3(0), planetradius + waterdepth + WaterLevel);
    vec3 atmorg = vec3(0, planetradius, 0) + CameraPosition;  
    Ray r = Ray(atmorg, dir);
    float shadowwater = 0.0;
    float hitdepth = 0.0;
    bool underwaterRoughness = false;
    float adf = 0.0;
    vec3 refr;
    float caustistics = 1.0;
    if(intersects(planethit)){ 
        Ray r = Ray(atmorg, dir);
        
        Sphere planet1 = Sphere(vec3(0), planetradius + WaterLevel);
        float planethit2 = intersectPlane(CameraPosition, dir, vec3(0.0, WaterLevel, 0.0), vec3(0.0, 1.0, 0.0));
        vec3 newpos = CameraPosition + dir * planethit;
        vec3 dfx = dFdx(newpos);
        vec3 dfy = dFdy(newpos);
        vec3 pixels = vec3(1.0 / Resolution.x, 1.0 / Resolution.y, 0.0);
        float dst = distance(CameraPosition, newpos);
        vec3 adfx = normalize(dir) * dst - reconstructCameraSpaceDistance(UV + pixels.xz, dst);
        vec3 adfy = normalize(dir) * dst - reconstructCameraSpaceDistance(UV + pixels.yz, dst);
        float df = length(dfx) + length(dfy);
        adf = length(adfx) + length(adfy);
        adf *= 0.1;
        float test = distance(CameraPosition, newpos) / length(Resolution.xy);
        //return (UV.x < 0.5 ? adf : df) * vec3(1);
        vec3 newpos2 = CameraPosition + dir * planethit2;
        //
        float flh1 = planethit * 0.001;
        roughness = smoothstep(0.0, 12.0, sqrt(sqrt(flh1)));
        dst = planethit;
        float lodz = pow(1.0 - smoothstep(0, LOD3, planethit), 1.0);
        vec3 n = vec3(0,1,0);
        float waveheight = 0.0;
        if(WaterWavesScale > 0.0){
            if(WaterWavesScale < 0.4){
                n = raymarchwaterHIGH3(newpos, newpos2);
            }
            else if(planethit >= LOD3) {
                n = raymarchwaterLOW3(newpos, newpos2);
            }
            else {
                vec3 lowres = raymarchwaterLOW3(newpos, newpos2);
                vec3 fullres = raymarchwater(newpos, newpos2, 14);
                n = mix(lowres, fullres, 1.0 - smoothstep(0, LOD3, planethit));
                //return  CSMQueryVisibility(newpos) * vec3(1);
            }
            shadowwater = CSMQueryVisibilityVariable(hitpos, 6, 2.0);
            /*float freq = 1.0;
            float h0 = heightwater(hitpos.xz, WATER_SAMPLES_HIGH);
            waveheight = h0;
            float h1 = heightwater(hitpos.xz + vec2(freq, 0.0), WATER_SAMPLES_HIGH);
            float h2 = heightwater(hitpos.xz + vec2(0.0, freq), WATER_SAMPLES_HIGH);
            float h3 = heightwater(hitpos.xz - vec2(freq, 0.0), WATER_SAMPLES_HIGH);
            float h4 = heightwater(hitpos.xz - vec2(0.0, freq), WATER_SAMPLES_HIGH);
            whites = max(0, h0 - (h1 + h2 + h3 + h4)*0.25) * 122.0 * pow(h0, 3.0);
            whites = min(1.0, pow(whites, 3.0));
            whites *= (1.0 - smoothstep(0, LOD3, planethit)) * WaterWavesScale;
            whites = min(1.0, whites);
            //return vec3(whites);
            float lodzx =  1.0 - clamp(hitdistx / LOD3, 0.0, 1.0);*/
            //roughness = roughness * 0.8 + 0.2;
           // 
           // return n * vec3(10.0, 0.0, 10.0);
        }
        vec3 newposW = CameraPosition + dir * hitdistx;
        vec3 origdir = dir;
        //return vec3(whites);
        roughness = adf * 0.44 * (WaterWavesScale);
        roughness = clamp(roughness, 0.0, 1.0);
        dir = normalize(reflect(dir, n));
        dir.y = abs(dir.y);
        refr = normalize(refract(origdir, n, 0.15));
        //return abs(refr);
        dir = mix(dir, reflect(origdir, vec3(0.0, 1.0, 0.0)), roughness);
        hitdepth = currentData.cameraDistance - hitdistx;
        vec3 newposr = hitpos + refr * hitdepth*200.0;
        //return vec3(fresnel);
        //defres += getAtmosphereForDirection(currentData.worldPos, n, normalize(SunDirection), currentData.roughness) * 0.5 * currentData.diffuseColor;
       // dir = normalize(mix(dir, vec3(0,1,0), roughness));
        
        fresnel = fresnel_again(vec3(0.04), n, dir, 1.0);
        if((hitdistx > 0 && hitdistx > currentData.cameraDistance) && ln > 0.01) {
           fresnel = 0.0;
        } else {
           underwaterRoughness = true;
            
           // return vec3(caustistics);
       }
       // defres = texture(directTex, uv).rgb + texture(alTex, uv).rgb * (UseAO == 1 ? texture(aoxTex, uv).r : 1.0);
       // basewaterclor = (1.0 - fresnel) * mix(vec3(0.0, 0.76, 0.55) * max(0, normalize(SunDirection).y * 0.9 + 0.1)* 0.1 * (waveheight * 0.3 + 0.7), defres, ln > 0.01 ? (1.0 - smoothstep(0.0, 33.0 * NoiseOctave7, hitdepth)) : 0.0);
        
       // fresnel = mix(fresnel, 0.02, sqrt(roughness));
      //return vec3(1) * (1.0 / (hitdepth * hitdepth * 0.03 + 1.0));
        //dir = normalize(reflect(dir, vec3(0,1,0)));
        doatmscatter = 0;
    } else {
        roughness = 0;//pow(1.0 - (dir.y), 128.0);
        shadowwater = 1.0;
    }
             //   return CSMQueryVisibilitySimple(currentData.worldPos) * vec3(1);
   // roughness = 0;
   //return shadowwater * vec3(1);
   //currentData = loadData(uv);
   if(underwaterRoughness) {
        hitdepth = currentData.cameraDistance - hitdistx;
        vec3 newposr = hitpos + refr * hitdepth*0.1;
        
        uvX =  abs(projectvdao(newposr));
        currentData = loadData(uvX);
        currentData.roughness = 1.0;
   }
   
   if(underwater || underwaterRoughness){
    float causshit = intersectPlane(currentData.worldPos, normalize(SunDirection), vec3(0.0, WaterLevel + waterdepth, 0.0), vec3(0.0, -1.0, 0.0));
    vec3 poscauss = currentData.worldPos + normalize(SunDirection) * causshit;
    vec3 ncauss = normalhix(poscauss.xyz, 0.5);
    vec3 refcauss = refract(-normalize(SunDirection), ncauss, 0.66);
    float dt = pow(dot(-refcauss, normalize(SunDirection)), 156.0);
    caustistics = pow(mix(1.0, causs(poscauss.xz * 0.01), min(1.0, WaterWavesScale)), 3.0) * (1.0 / (1.0 + causshit / NoiseOctave7 * 0.1));
    //return caustistics * vec3(1);
   }

   
    float cwp = CSMQueryVisibility(currentData.worldPos) * (max(0, caustistics));
    //return cwp * vec3(1);
    //return MakeShading(currentData) * vec3(1);
   // return MakeShading(currentData) * cwp;
   // spaghetti incoming
    vec3 defres = ln < 0.01 ? vec3(0) : (texture(directTex, uvX).rgb + texture(alTex, uvX).rgb * (UseAO == 1 ? texture(aoxTex, uvX).r : 1.0) + MakeShading(currentData) * cwp + currentData.diffuseColor * 0.009 * litcolor2 * (UseAO == 1 ? texture(aoxTex, uvX).r : 1.0));
    
    vec3 mixing = vec3(
        1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(hitdepth) * pow(hitdepth, 1.4)),
        1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(hitdepth) * pow(hitdepth, 1.3)),
        1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(hitdepth) * pow(hitdepth, 1.2))
    );
    /*
    if(underwater) {
        float dst = planethit > 0 ? min(currentData.cameraDistance, planethit) : currentData.cameraDistance;
        mixing = vec3(
            1.0 - smoothstep(0.0, 33.0 * NoiseOctave7, sign(dst) * pow(dst, 1.4)),
            1.0 - smoothstep(0.0, 33.0 * NoiseOctave7, sign(dst) * pow(dst, 1.2)),
            1.0 - smoothstep(0.0, 33.0 * NoiseOctave7, dst)
        );
    }*/
    
    if(!underwater) defres = (1.0 - fresnel) * mix(vec3(0.0, 0.76, 0.55) * 0.1 * max(0, normalize(SunDirection).y * 0.9), defres, (ln > 0.01) ? (mixing) : vec3(0.0));
    currentData = loadData(UV);

    vec4 cdata = smartblur(dir, roughness).rbga;
    //if(UV.x < 0.5) return cdata.bbb * 0.00004;
    //return cdata.aaa;
   // return pow(vec3(cdata.b) * 0.0003, vec3(2.0));
   // return cdata.ggg;
    //cdata.a = doatmscatter ;//* clamp(pow(cdata.a * 1.0, 16.0) * 1.0, 0.0, 1.0);
    vec3 skydaylightcolor = vec3(0.23, 0.33, 0.48) * 1.3;
    vec3 sunx = sun(dir, normalize(SunDirection),  min(1.0, 1.0 / max(0.01, adf * adf * 0.1 * WaterWavesScale * (NoiseOctave2 * NoiseOctave3)))) * vec3(atmcolor) * 40.0 * shadowwater;
    vec3 scatt = getAtmosphereForDirection(dir, roughness) + sunx;
    vec3 atmcolor1 = getAtmosphereForDirection(vec3(0,1,0), 0.0);
    float dirdirdir = pow(16.0, max(0, dot(normalize(SunDirection), normalize(dir))));
    float diminisher_absolute = dot(normalize(SunDirection), vec3(0,1,0)) * 0.5 + 0.5;
    
    float dimpow = 1.0 -  diminisher;
    float dmxp = max(0.01, pow(1.0 - max(0, -normalize(SunDirection).y), 32.0));
    

    vec3 shadowcolor = mix(skydaylightcolor, skydaylightcolor * 0.05, 1.0 - diminisherX);
    vec3 scatcolor = mix(vec3(1.0), atmcolor, 1.0 - diminisher) ;
    vec3 litcolor = mix(vec3(3.0) + dirdirdir * 0.2   , atmcolor * 0.2, pow(1.0 - diminisherX, 3.0));
    vec3 colorcloud = dmxp * mix(shadowcolor, litcolor, cdata.g) ;//* (diminisher * 0.3 + 0.7);
    
        //    return sun(dir, normalize(SunDirection), 1.0 - roughness );
   // cdata.r = mix(0.0, cdata.r, 1.0 - pow(1.0 - dir.y, 45.0));
    //   cdata.r = mix(cdata.r, 0.0, min(1.0, dst * 0.000005));
    //cdata.a = mix(1.0, cdata.a, 1.0 - pow(1.0 - dir.y, 24.0));
    float xv  = 0.0;//min(1.0, cdata.b * 0.00001) * max(0.0, sqrt(sqrt(normalize(SunDirection).y)));
    
    vec3 scatcolorsky = cdata.a * scatcolor;
    //return cdata.aaa;
    vec3 result = fresnel * mix(scatt, colorcloud, min(1.0, cdata.r * 1.2)) + fresnel * cdata.a * scatcolor;
    result = clamp(result, vec3(0.0), vec3(300.0));
    
   // if(underwater) {
        //result = mix(vec3(0.0, 0.76, 0.55) * 0.1 * max(0, normalize(SunDirection).y * 0.9), result, mixing);
        
  //  }
    //return sunx;
   // result += cdata.aaa * atmcolor * fresnel;// + diminisher_absolute * (0.5 * pow(diminisher, 8.0) + 0.5) * litcolor * ((pow(1.0 - diminisher, 24.0)) * 0.9 + 0.1) * pow(cdata.a * 1.0, 2.0);
  //  if(UV.x < 0.5)return cdata.aaa;
    //return vec3(hitdistx);
  //  return result;
   // return texture(atmScattTex, UV).rgb;
   //if(hitdistx > 0 && hitdistx < currentData.cameraDistance || ln < 0.01) 
   //defres += gimmegodrays(CameraPosition, normalize(reconstructCameraSpaceDistance(UV, 1.0) ));
  // else 
  //return textureLod(fogTex, UV, 0).ggg;
  // defres = mix(defres, scatcolor * textureLod(fogTex, UV, 0).r, 1.0 - clamp(textureLod(fogTex, UV, 0).g, 0.0, 1.0));
   //defres = CSMQueryVisibilitySimple(currentData.worldPos) * vec3(1);
   //return result;
   //if(underwater) return vec3(0.1);
   //if((hitdistx <= 0 && hitdistx > csdr && planethit > 0 && )&& ln > 0.01)defres *= 0.5 + 0.5    * underwaterPercentage(currentData.worldPos);
   
    //float uperct = max(0.0, heightwater(currentData.worldPos.xz, WATER_SAMPLES_LOW, -0.2) * waterdepth + WaterLevel - currentData.worldPos.y);
   // defres *= uperct;
    
  if(currentData.cameraDistance == 0) currentData.cameraDistance = min(99999.0, hitdistx);
  float csdr = currentData.cameraDistance;
  currentData.cameraDistance = min(currentData.cameraDistance, hitdistx);
   float minshadow = shadowwater * 0.3 + 0.7;
   //return csdr * vec3(0.001);
   if(!underwater){
       if( ln < 0.01) 
        result = result * minshadow + defres * minshadow;
       else if(hitdistx > 0 && hitdistx > csdr && ln > 0.01) 
            result = defres;
       else if(hitdistx > 0 && hitdistx < csdr && ln > 0.01){
            result = result * minshadow + defres * minshadow;
       } else result = defres;//mix(result, vec3(0.7), ;
       result = mix(result, (ln < 0.01 && hitdistx <= 0 ? result : vec3(0)) + litcolor * 0.08 * textureLod(fogTex, UV, 0).r, 1.0 - clamp(textureLod(fogTex, UV, 0).g, 0.0, 1.0));
   } else {
   //if(underwater) {
        float dst = csdr;
        dst = dst == 0.0 ? 99999.0 : dst;
        mixing = vec3(
            1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(dst) * pow(dst, 1.4)),
            1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(dst) * pow(dst, 1.3)),
            1.0 - smoothstep(0.0, 93.0 * NoiseOctave7, sign(dst) * pow(dst, 1.2))
        );
    //} 
    //return csdr * vec3(0.001);
   // return defres + result;
    return mix(vec3(0.0, 0.76, 0.55) * 0.1 * max(0, normalize(SunDirection).y * 0.9), defres + result, mixing);
    //return defres + result;
       if( ln < 0.01) 
        result = result * minshadow + defres * minshadow;
       else if(hitdistx > 0 && hitdistx > csdr && ln > 0.01) 
            result = defres;
       else if(hitdistx > 0 && hitdistx < csdr && ln > 0.01){
            result = result * minshadow + defres * minshadow;
       } else result = defres;//mix(result, vec3(0.7), ;
       result = mix(result, (ln < 0.01 && hitdistx <= 0 ? result : vec3(0)) + litcolor * 0.08 * textureLod(fogTex, UV, 0).r, 1.0 - clamp(textureLod(fogTex, UV, 0).g, 0.0, 1.0));   
   }
   return result;
   //return texture(atmScattTex, UV).rgb;
  // return texture(atmScattTex, UV).rgb;
  // eturn getatscatter(dir, normalize(SunDirection))+ sun(dir, normalize(SunDirection));
   //    return vec3(1)  * cdata.g;
}

vec3 fisheye(){
    vec2 fullsp = UV * 2.0 - 1.0;
    //vec3 dir = normalize(reconstructCameraSpaceDistance(UV * 0.5 + 0.5, 1.0));
  //  fullsp = fullsp / sqrt(1.0 - length(fullsp) * 0.71);
    //fullsp *= 0.5;
    vec3 rld = normalize(reconstructCameraSpaceDistance(vec2(0.5), 1.0));
    vec3 dir = normalize(reconstructCameraSpaceDistance(fullsp * 0.5 + 0.5, 1.0));
    vec3 xdir = rld - dir;
   // xdir.y *= 0.15;
    dir -= xdir * 2.5 * NoiseOctave4;
   // dir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    //vec3 dir = normalize(reconstructCameraSpaceDistance((fullsp * 3.0) * 0.5 + 0.5, 1.0));
    return normalize(dir);
}

vec4 shade(){    
    vec3 color = cloudsbydir(fisheye());
    return vec4( color, currentData.cameraDistance);
}