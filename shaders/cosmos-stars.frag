#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform UniformBufferObject1 {
    float Time;
    float Zero;
    vec2 Mouse;
    mat4 VPMatrix;
    vec4 inCameraPos;
    vec4 inFrustumConeLeftBottom;
    vec4 inFrustumConeBottomLeftToBottomRight;
    vec4 inFrustumConeBottomLeftToTopLeft;
} hiFreq;

layout(set = 0, binding = 1) uniform UniformBufferObject2 {
    vec4 NoiseSeed_Size_ZeroZero;
    vec4 Position;
} starBuf;

float NoiseSeed = starBuf.NoiseSeed_Size_ZeroZero.r;
float StarSize = starBuf.NoiseSeed_Size_ZeroZero.g;
vec3 starPosition = starBuf.Position.rgb;
float Time = hiFreq.Time;

vec3 CameraPosition = hiFreq.inCameraPos.xyz;
vec3 FrustumConeLeftBottom = hiFreq.inFrustumConeLeftBottom.xyz;
vec3 FrustumConeBottomLeftToBottomRight = hiFreq.inFrustumConeBottomLeftToBottomRight.xyz;
vec3 FrustumConeBottomLeftToTopLeft = hiFreq.inFrustumConeBottomLeftToTopLeft.xyz;

#include camera.glsl

struct Ray { vec3 o; vec3 d; };
struct Sphere { vec3 pos; float rad; };
float rsi2_simple(in Ray ray, in Sphere sphere)
{
    vec3 oc = ray.o - sphere.pos;
    float b = 2.0 * dot(ray.d, oc);
    return -b - sqrt(b * b - 4.0 * (dot(oc, oc) - sphere.rad*sphere.rad));
}
vec2 rsi2(in Ray ray, in Sphere sphere)
{
    vec3 oc = ray.o - sphere.pos;
    float b = 2.0 * dot(ray.d, oc);
    float c = dot(oc, oc) - sphere.rad*sphere.rad;
    float disc = b * b - 4.0 * c;
    vec2 ex = vec2(-b - sqrt(disc), -b + sqrt(disc))/2.0;
    return vec2(min(ex.x, ex.y), max(ex.x, ex.y));
}
mat3 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat3(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s,
        oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s,
        oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c);
}
float hash( float n ){
    return fract(sin(n)*758.5453);
}
float noise2d( in vec2 x ){
    vec2 p = floor(x);
    vec2 f = smoothstep(0.0, 1.0, fract(x));
    float n = p.x + p.y*57.0;
    return mix(
	    mix(hash(n+0.0),hash(n+1.0),f.x),
	    mix(hash(n+57.0),hash(n+58.0),f.x),
	    f.y
	   );
}
float noise3d( in vec3 x ){
	vec3 p = floor(x);
    	vec3 f = smoothstep(0.0, 1.0, fract(x));
	float n = p.x + p.y*157.0 + 113.0*p.z;

	return mix(mix(	mix( hash(n+0.0), hash(n+1.0),f.x),
			mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
		   mix(	mix( hash(n+113.0), hash(n+114.0),f.x),
			mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}
// YOU ARE WELCOME! 4d NOISE
float noise4d(vec4 x){
	vec4 p=floor(x);
	vec4 f=smoothstep(0.,1.,fract(x));
	float n=p.x+p.y*157.+p.z*113.+p.w*971.;
	return mix(mix(mix(mix(hash(n),hash(n+1.),f.x),mix(hash(n+157.),hash(n+158.),f.x),f.y),
	mix(mix(hash(n+113.),hash(n+114.),f.x),mix(hash(n+270.),hash(n+271.),f.x),f.y),f.z),
	mix(mix(mix(hash(n+971.),hash(n+972.),f.x),mix(hash(n+1128.),hash(n+1129.),f.x),f.y),
	mix(mix(hash(n+1084.),hash(n+1085.),f.x),mix(hash(n+1241.),hash(n+1242.),f.x),f.y),f.z),f.w);
}
float FBM2(vec2 p, int octaves, float dx){
	float a = 0.0;
    	float w = 0.5;
	for(int i=0;i<octaves;i++){
		a += noise2d(p) * w;
        	w *= 0.5;
		p *= dx;
	}
	return a;
}
float FBM3(vec3 p, int octaves, float dx){
	float a = 0.0;
    	float w = 0.5;
	for(int i=0;i<octaves;i++){
		a += noise3d(p) * w;
        	w *= 0.5;
		p *= dx;
	}
	return a;
}
float FBM4(vec4 p, int octaves, float dx){
	float a = 0.0;
    	float w = 0.5;
	for(int i=0;i<octaves;i++){
		a += noise4d(p) * w;
        	w *= 0.5;
		p *= dx;
	}
	return a;
}

float rand2s(vec2 co){
    return fract(sin(dot(co.xy * Time,vec2(12.9898,78.233))) * 43758.5453);
}
float rand3s(vec3 co){
    return fract(sin(dot(co.xyz * Time,vec3(12.9898,78.233,138.1246))) * 43758.5453);
}

Sphere surface;
Sphere atmosphere;
Ray cameraRay;
vec3 starColor;
vec2 uvseed;

vec4 traceStarAtmosphere(vec3 start, vec3 end, float lengthstart, float lengthstop){
	vec4 result = vec4(0.0);
	const int steps = 64;
	float stepsize = 1.0 / float(steps);
	float iter = rand2s(uvseed) * stepsize;;
	for(int i=0;i<steps;i++){
		vec3 p = mix(start, end, iter);
		float len = length(p);
		vec3 heatcast = normalize(p);
		float mx = (smoothstep(lengthstart, lengthstop, len));
		float n = stepsize * 4.0 * (1.0 - mx) * smoothstep(sqrt(mx) * 0.2 + 0.5, sqrt(sqrt(mx)) * 0.5 + 0.5, FBM4(vec4(heatcast * 15.0, 0.1 * Time - (len * 1.3 - 1.0)), 3, 2.0));
		result.xyz += starColor * (1.0 - result.a) * n * 2.0;
		result.a += n * 0.6;
		if(result.a > 1.0) break;
		iter += stepsize;
	}
	result.a = min(1.0, result.a);
	return result;
}
#define hits(a) (a>0.0&&a<999.0)
#define seed 13.0
vec4 traceStar(vec3 position, float size, float atmospheresize){
	surface = Sphere(position, size);
	atmosphere = Sphere(position, atmospheresize);
	vec3 color = vec3(0.0);
	float coverage = 0.0;
	starColor = vec3(0.7)+0.3 * vec3(hash(seed), hash(seed + 100.0), hash(seed + 200.0));

	float hit_Surface = rsi2(cameraRay, surface).x;
	vec2 hit_Atmosphere = vec2(hit_Surface);//rsi2(cameraRay, atmosphere);
	/*3 possible scenarios:
	nothing hit
	only atmosphere hit
	atmosphere and surface hit
	nothing is going to hit from the inside, you are not going into the star ok..
	*/
    float dtraw = dot(normalize(position - cameraRay.o), cameraRay.d);
    float dotz = max(0.0, dtraw);
    float dotf = dtraw * 0.5 + 0.5;

    float sqrlen = length(position) * length(position);
    float specialtrick1 = 1.0 / (1.0 + 1.0 * sqrlen * (1.0 - dotz));

    color += (1.0 / 128.0) * vec3(rand2s(uvseed));

	if(!hits(hit_Surface) && !hits(hit_Atmosphere.x)) {

        color += specialtrick1 * starColor * 5.0;
        return vec4(color, length(position));

    }
	if(!hits(hit_Surface) && hits(hit_Atmosphere.x)){
		// Only ATMOSPHERE hit
		//vec4 atm = traceStarAtmosphere(cameraRay.o + cameraRay.d * hit_Atmosphere.x, cameraRay.o + cameraRay.d * hit_Atmosphere.y, size, atmospheresize);
		//color = atm.xyz * 5.0;
		//coverage = atm.a;
	}
	if(hits(hit_Surface) && hits(hit_Atmosphere.x)) {
		//vec4 atm = traceStarAtmosphere(cameraRay.o + cameraRay.d * hit_Atmosphere.x, cameraRay.o + cameraRay.d * hit_Surface, size, atmospheresize);
		vec3 directsun = starColor * 5.0 * (1.0 - smoothstep(0.65, 0.7, FBM4(vec4(((cameraRay.o + cameraRay.d * hit_Surface) - position) * 4.0, Time * 0.1), 2, 2.0)));
		//color = mix(directsun, atm.xyz, atm.w);
        color += directsun;
		coverage = 1.0;
	}


	return vec4(color, length(position));
}

float starSize = StarSize;
float starAtmosphere = StarSize + 0.9;


float supernoise3dX(vec3 p){

	float a =  noise3d(p);
	float b =  noise3d(p + 0.5);
	return (a * b);
}
float fbmHI2d(vec3 p, float dx){
   // p *= 0.1;
    p *= 1.2;
	//p += getWind(p * 0.2) * 6.0;
	float a = 0.0;
    float w = 1.0;
    float wc = 0.0;
	for(int i=0;i<5;i++){
        //p += noise(vec3(a));
		a += clamp(2.0 * abs(0.5 - (supernoise3dX(vec3(p.xy, p.z + Time * 0.2)))) * w, 0.0, 1.0);
		wc += w;
        w *= 0.5;
		p = p * dx;
	}
	return a / wc;// + noise(p * 100.0) * 11;
}
float mod289(float x){return mod(x, 289.0);}
vec4 mod289(vec4 x){return mod(x, 289.0);}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = smoothstep(0.0, 1.0, p - a);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (0.02439024));
    vec4 o2 = fract(k4 * (0.02439024));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}
float stars(vec3 seedd, float intensity){
    float stepstart = 1.0 - intensity*0.3;
    float stepend = min(1.0, stepstart + 0.1);
	return smoothstep(stepstart, stepend, noise(-seedd * 600.0));
}/*
vec3 stars(vec3 uv){
	float intensityred = (1.0 / (1.0 + 30.0 * abs(uv.y))) * fbmHI2d(uv * 30.0, 3.0) * (1.0 - abs(uv.x ));
	float intensitywhite = (1.0 / (1.0 + 20.0 * abs(uv.y))) * fbmHI2d(uv * 30.0 + 120.0, 3.0) * (1.0 - abs(uv.x ));
	float intensityblue = (1.0 / (1.0 + 20.0 * abs(uv.y))) * fbmHI2d(uv * 30.0 + 220.0, 3.0) * (1.0 - abs(uv.x ));
	float galaxydust = smoothstep(0.1, 0.5, (1.0 / (1.0 + 20.0 * abs(uv.y))) * fbmHI2d(uv * 20.0 + 220.0, 3.0) * (1.0 - abs(uv.x )));
	float galaxydust2 = smoothstep(0.2, 0.5, (1.0 / (1.0 + 20.0 * abs(uv.y))) * fbmHI2d(uv * 50.0 + 220.0, 3.0) * (1.0 - abs(uv.x )));
	intensityred = 1.0 - pow(1.0 - intensityred, 3.0) * 0.73;
	intensitywhite = 1.0 - pow(1.0 - intensitywhite, 3.0) * 0.73;
	intensityblue = 1.0 - pow(1.0 - intensityblue, 3.0) * 0.73;
	float redlights = stars(uv - 500.0, intensityred );
	float whitelights = stars(uv, intensitywhite );
	float bluelights = stars(uv - 200.0, intensityblue );
	vec3 starscolor = vec3(1.0, 0.8, 0.5) * redlights + vec3(1.0) * whitelights + vec3(0.6, 0.7, 1.0) * bluelights;
	vec3 dustinner = vec3(0.8, 0.9, 1.0);
	vec3 dustouter = vec3(0.2, 0.1, 0.0);
	vec3 innermix = mix(dustinner, starscolor, 1.0 - galaxydust);
	vec3 bloom = dustinner * (1.0 / (1.0 + 30.0 * abs(uv.y))) * fbmHI2d(uv * 3.0, 3.0) * (1.0 - abs(uv.x ));
	vec3 allmix = mix(dustouter, innermix + bloom, 1.0 - galaxydust2);
	return allmix + bloom;
}*/

vec3 milkyway(vec3 uv){
    mat3 r = rotationMatrix(vec3(1.0), 12.1244);
	return vec3(1.0) * stars(r * uv, 0.8) * 0.3;
}
void main() {
	uvseed = UV;
	cameraRay = Ray(vec3(0.0), reconstructCameraSpaceDistance(UV, 1.0));
	outColor = traceStar(starPosition, starSize, starAtmosphere);// + vec4(milkyway(cameraRay.d), 0.0);
}
