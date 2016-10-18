#version 430 core


layout(binding = 3) uniform samplerCube skyboxTex;
layout(binding = 5) uniform sampler2D directTex;
layout(binding = 6) uniform sampler2D alTex;
layout(binding = 16) uniform sampler2D aoxTex;
layout(binding = 20) uniform sampler2D fogTex;
layout(binding = 21) uniform sampler2D waterColorTex;
layout(binding = 22) uniform sampler2D inputTex;

layout(binding = 25) uniform samplerCube coverageDistTex;
layout(binding = 26) uniform samplerCube shadowsTex;
layout(binding = 27) uniform samplerCube skyfogTex;
layout(binding = 28) uniform sampler2D moonTex;
layout(binding = 23) uniform sampler2D waterTileTex;
layout(binding = 24) uniform sampler2D starsTex;

uniform int UseAO;

uniform int CombineStep;
#define STEP_PREVIOUS_SUN 0
#define STEP_WATER_CLOUDS 1

#include Constants.glsl
#include PlanetDefinition.glsl
#include PostProcessEffectBase.glsl
#include Atmosphere.glsl
#include noise2D.glsl
#include CSM.glsl
#include FXAA.glsl
#include ResolveAtmosphere.glsl

vec3 integrateStepsAndSun(){
    currentData.cameraDistance = length(currentData.normal) < 0.01 ? 999999.0 : currentData.cameraDistance;
    return getNormalLighting(UV, currentData);
}

vec3 integrateCloudsWater(){
    return fxaa(waterColorTex, UV).rgb;
}


#define dist 0.05
#define zoom 15.0

vec2 mouse = projectvdao(CameraPosition + dayData.sunDir * 100.0);
vec2 resolution = Resolution;

vec3 tex2D(vec2 uv)
{
	if (uv.x == 0.0 || uv.y == 0.0 || uv.x == 1.0 || uv.y == 1.0) return vec3(0.0);
	
	float d = distance(uv, mouse) ;
	if (d >= dist) return vec3(0.0);
	
	return  vec3(0.2 * ((dist-d)/dist), 0.4 * ((dist-d) / dist), 0.8 * ((dist - d) / dist));
}

vec3 chroma( ) 
{
	vec2 uv = UV;
    
		
	float tt = 1.0 / abs( distance(uv, mouse) * zoom );
	
	float v = 1.0 / abs( length((mouse-uv) * vec2(0.03, 1.0)) * (zoom * 10.0) );
	
	vec3 finalColor = tex2D(uv) * 0.5;
	//finalColor += vec3( 2.0 * tt, 4.0 * tt, 8.0 * tt);
	//finalColor += vec3( 2.0 * v, 4.0 * v, 8.0 * v );
	
	//ghost
	float x;
	uv = gl_FragCoord.xy / resolution.xy - 0.5;
	x = length(uv);
	uv *= pow(x,4.0) * -100.0 + 1.0 / (x - 0.5);
	uv = clamp(uv + 0.5, 0.0, 1.0);
	finalColor += tex2D(uv);
	
	//ghost with double chroma
	uv = gl_FragCoord.xy / resolution.xy - 0.5;
	x = length(uv);
	uv *= pow(x, 16.0) * -1000000.0 + 0.2/(x - 0.3);
	uv = clamp(uv + 0.5, 0.0, 1.0);
	finalColor += tex2D(uv);
	
	//chroma
	uv = gl_FragCoord.xy / resolution.xy - 0.5;
	x = length(uv);
	uv *= pow(x, 16.0) * -20000.0 + 0.2 / (x * x + 5.0);
	uv = clamp(uv + 0.5, 0.0, 1.0);
	//finalColor += tex2D(uv);
	
	//double chroma
	uv = gl_FragCoord.xy / resolution.xy-0.5;
	x = length(uv);
	uv *= pow(x, 16.0) * -10000.0 + 0.2 / (x * x);
	uv = clamp(uv + 0.5, 0.0, 1.0);
	//finalColor += tex2D(uv);

	
	vec2 D = 0.5 - gl_FragCoord.xy / resolution.xy; 
	vec3 o = vec3(-D.x * 0.4, 0.0, D.x * 0.4);

	vec3 lx = vec3(0.01, 0.01, 0.3);

	vec2 S = gl_FragCoord.xy / resolution.xy - 0.5;
	vec2 m = 0.5 * S;
	m.xy *= pow(4.0 * length(vec2(S.x,S.y)), 1.0);
	m.xy *= -2.0;
	m.xy = 0.5 + m.xy;

	vec3 e = tex2D(m.xy);

	S = (m.xy - 0.5) * 1.75;
	e *= clamp(1.0 - dot(S,S), 0.0, 1.0);
	
	float n = max(e.x, max(e.y, e.z)), c = n / (1.0 + n);
		
	e.xyz *= c;
	finalColor += e;

	return finalColor;

}

vec4 shade(){    
    vec3 color = vec3(0);
    if(CombineStep == STEP_PREVIOUS_SUN){
        color = integrateStepsAndSun();
    } else {
        color = integrateCloudsWater() + chroma();
    }
    return vec4( clamp(color, 0.0, 10.0), currentData.cameraDistance * 0.001);
}