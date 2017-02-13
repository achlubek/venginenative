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
layout(binding = 29) uniform samplerCube resolvedAtmosphereTex;

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

vec3 integrateStepsAndSun(vec3 dir){
	return sampleAtmosphere(dir, 0.0, 1.0, 111);
}

vec4 shade(){
    vec3 color = vec3(0);
	vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
	//if(dir.y < 0.0){
	//	return textureLod(atmScattTex, vec3(dir.x, -dir.y, dir.z), textureQueryLevels(atmScattTex)).rgba  ;
	//}
	//dir.y = abs(dir.y);
	color = integrateStepsAndSun(dir);
    if(dir.y < -0.01) color *= 0.4;
    return vec4( clamp(color, 0.0, 1110.0), textureLod(coverageDistTex, dir, 0.0).r);
}
