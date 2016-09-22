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

uniform int UseAO;
uniform float T100;
uniform float T001;

#define CLOUD_SAMPLES 18
#define CLOUDCOVERAGE_DENSITY 50    
#include Atmosphere.glsl
float hitdistx;
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
float blurshadowabit(vec3 uv, float d){
    float v = 0.0;
    const float w = 1.0 / 12.0;
    for(int i=0;i<12;i++){
        v += smoothstep(0.0, 0.0012, d - texture(CSMTex, uv + randpointx() * 0.00115).r);
      //  v += 1.0 - max(0, sign(d - texture(CSMTex, uv + randpointx() * 0.00115).r));
    }
    return v * w;
}
float giscale = 1.0;
float blurshadowabitGI(vec3 uv, float d){
    float v = 0.0;
    const float w = 1.0 / 64.0;
    for(int i=0;i<64;i++){
        v += smoothstep(0.0, 0.0012, d - texture(CSMTex, uv + randpointx() * 11.6115 * giscale).r);
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
            return 1.0 - blurshadowabit(vec3(csmuv.x, csmuv.y, float(i)), d);// abs(csmuv.z - texture(CSMTex, csmuv).r) ;
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

#define WaterLevel (0.01+ 10.0 * NoiseOctave6)
vec2 tracegodrays(vec3 p1, vec3 p2){
    float rays = 0.0;
    float coverage = 1.0;
    int steps = int(clamp(floor(distance(p1, p2) * 0.2), 17.0, 32.0));
    float stepsize = 1.0 / float(steps);
    float rd = rand2s(UV * Time) * stepsize;
    float iter = 0.0;
    vec3 lastpos = p1;
    float floorh = WaterLevel;
    float highh = 100.0 * NoiseOctave8;
    float cloudslow = planetradius + floorh;
    float cloudshigh = planetradius + highh;
    vec3 d = normalize(SunDirection);
    float w = 0.0;
    for(int i=0;i<steps;i++){
        vec3 p = mix(p1, p2, rd + iter);
        Ray r = Ray(vec3(0, planetradius, 0) + p, d);
       // float planethit = max(0, sign(-rsi2(r, planet)));
        float dist = distance(p, lastpos);
        float height = length(vec3(0, planetradius, 0) + p);
        lastpos = p;
        float xz  = (1.0 - smoothstep(cloudslow, cloudshigh, height));
        float z = mix(noise(p.xyz*0.015 + Time * 0.1), 1.0, pow(xz, 3.0));
        rays += coverage * (dist * CSMQueryVisibilitySimple(p) + dist * 0.15);
        w += coverage;
        coverage *= 1.0 - max(z * dist * xz * NoiseOctave5 * 0.0004, 0.0);
        if(coverage <= 0.0) break;
        iter += stepsize;
    }
    return vec2(clamp(rays / w, 0.0, 999.0), clamp(coverage, 0.0, 1.0));
}

#define waterdepth 1.0 * WaterWavesScale
vec2 gimmegodrays(vec3 o, vec3 d){
    vec2 rays = vec2(0.0);

    float floorh = WaterLevel + waterdepth;
    float highh = 100.0 * NoiseOctave8;
    
    float cloudslow = planetradius + floorh;
    float cloudshigh = planetradius + highh;
    
    Ray r = Ray(vec3(0, planetradius, 0) + o, d);
    float height = length(vec3(0, planetradius, 0) + o);
    
    sphere1 = Sphere(vec3(0), planetradius + floorh);
    sphere2 = Sphere(vec3(0), planetradius + highh);
        
    float planethit = rsi2(r, planet);
    float hitfloor = rsi2(r, sphere1);
    float floorminhit = minhit;
    float floormaxhit = maxhit;
    float hitceil = rsi2(r, sphere2);
    float ceilminhit = minhit;
    float ceilmaxhit = maxhit;
    float dststart = 0.0;
    float dstend = 0.0;
    float lim = hitdistx > 0 ? min(currentData.cameraDistance, hitdistx) : currentData.cameraDistance;
    //return lim * 0.1;
    planethit = min(lim , planethit);
    hitfloor = min(lim, hitfloor);
    hitceil = min(lim, hitceil);
    floorminhit = min(lim, floorminhit);
    floormaxhit = min(lim, floormaxhit);
    ceilminhit = min(lim, ceilminhit);
    ceilmaxhit = min(lim, ceilmaxhit);
    minhit = min(lim, minhit);
    maxhit = min(lim, maxhit);
    if(height < cloudslow){
        if(planethit < 0){
            rays = tracegodrays(o + d * hitfloor, o + d * hitceil);
        }
    } else if(height >= cloudslow && height < cloudshigh){
        if(intersects(hitfloor)){
            rays = tracegodrays(o, o + d * floorminhit);
        } else {
            rays = tracegodrays(o, o + d * hitceil);
        }
    } else if(height >= cloudshigh){
        if(!intersects(hitfloor) && !intersects(hitceil)){
            rays = vec2(0.0, 1.0);
        } else if(!intersects(hitfloor)){
            rays = tracegodrays(o + d * minhit, o + d * maxhit);
        } else {
            rays = tracegodrays(o + d * ceilminhit, o + d * floorminhit);
        }
    }
    return rays;
}

vec4 shade(){  
//    vec2 pixels = 1.0 / Resolution;
//    pixels *= 0.5;
 //   currentData = loadData(UV + pixels);
    if(currentData.cameraDistance < 0.01) currentData.cameraDistance = max(999999.0, hitdistx);
    vec2 color = NoiseOctave5 > 0.011 ? (gimmegodrays(CameraPosition, normalize(reconstructCameraSpaceDistance(UV, 1.0) ))) : vec2(0.0, 1.0);
    return vec4( color.x, color.y, 0.0, 1.0);
}